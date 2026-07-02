/* ArcLight Engine - Blueprint Visual Scripting System
 * Developer: Stephen
 * Node-based visual scripting with compilation to bytecode.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>

namespace arclight {

enum class EBlueprintNodeType { Entry, Event, Function, Branch, Print, Math, Custom };
enum class EPinType { Bool, Int, Float, String, Vector, Exec, Unknown };
enum class EPinDirection { Input, Output };

struct BlueprintPin {
	std::string id;
	std::string name;
	EPinDirection direction = EPinDirection::Input;
	EPinType type = EPinType::Unknown;
};

struct BlueprintNode {
	std::string id;
	std::string name;
	EBlueprintNodeType type = EBlueprintNodeType::Custom;
	float x = 0, y = 0;
	std::vector<BlueprintPin> inputPins;
	std::vector<BlueprintPin> outputPins;
};

struct BlueprintVariable {
	std::string name;
	EPinType type = EPinType::Unknown;
	std::string defaultValue;
};

struct BlueprintConnection {
	std::string id;
	std::string fromNodeID, fromPinID;
	std::string toNodeID, toPinID;
};

class BlueprintGraph {
public:
	std::string name;
	std::vector<std::unique_ptr<BlueprintNode>> nodes;
	std::vector<BlueprintConnection> connections;
	std::vector<BlueprintVariable> variables;
	uint32_t nextNodeID = 1;

	BlueprintNode* AddNode(EBlueprintNodeType type, const std::string& nodeName, float x, float y) {
		auto node = std::make_unique<BlueprintNode>();
		node->id = "node_" + std::to_string(nextNodeID++);
		node->name = nodeName;
		node->type = type;
		node->x = x; node->y = y;
		if (type == EBlueprintNodeType::Entry || type == EBlueprintNodeType::Event)
			node->outputPins.push_back({"exec_out", "Exec", EPinDirection::Output, EPinType::Exec});
		if (type == EBlueprintNodeType::Print) {
			node->inputPins.push_back({"exec_in", "Exec", EPinDirection::Input, EPinType::Exec});
			node->inputPins.push_back({"text", "Text", EPinDirection::Input, EPinType::String});
			node->outputPins.push_back({"exec_out", "Exec", EPinDirection::Output, EPinType::Exec});
		}
		if (type == EBlueprintNodeType::Branch) {
			node->inputPins.push_back({"exec_in", "Exec", EPinDirection::Input, EPinType::Exec});
			node->inputPins.push_back({"condition", "Condition", EPinDirection::Input, EPinType::Bool});
			node->outputPins.push_back({"true", "True", EPinDirection::Output, EPinType::Exec});
			node->outputPins.push_back({"false", "False", EPinDirection::Output, EPinType::Exec});
		}
		BlueprintNode* ptr = node.get();
		nodes.push_back(std::move(node));
		return ptr;
	}

	BlueprintConnection* Connect(const std::string& fromNode, const std::string& fromPin,
		const std::string& toNode, const std::string& toPin) {
		BlueprintConnection conn;
		conn.fromNodeID = fromNode; conn.fromPinID = fromPin;
		conn.toNodeID = toNode; conn.toPinID = toPin;
		connections.push_back(conn);
		return &connections.back();
	}

	void AddVariable(const BlueprintVariable& var) { variables.push_back(var); }

	BlueprintNode* FindNode(const std::string& nodeID) {
		for (auto& n : nodes) if (n->id == nodeID) return n.get();
		return nullptr;
	}

	bool IsValid() const {
		for (auto& n : nodes) if (n->type == EBlueprintNodeType::Entry) return true;
		return false;
	}
};

class BlueprintCompiler {
public:
	std::vector<uint8_t> bytecode;
	bool compilationSuccess = false;

	bool Compile(BlueprintGraph& graph) {
		bytecode.clear();
		if (!graph.IsValid()) return false;
		for (auto& node : graph.nodes) {
			if (node->type == EBlueprintNodeType::Entry) {
				bytecode.push_back(0x01); // OP_ENTRY
				for (auto& conn : graph.connections) {
					if (conn.fromNodeID == node->id) {
						bytecode.push_back(0x20); // OP_PRINT
					}
				}
			}
		}
		compilationSuccess = !bytecode.empty();
		return compilationSuccess;
	}

	size_t GetBytecodeSize() const { return bytecode.size(); }
};

class BlueprintSystem {
public:
	std::vector<std::unique_ptr<BlueprintGraph>> graphs;
	BlueprintCompiler compiler;
	uint32_t nextGraphID = 1;

	BlueprintGraph* CreateGraph(const std::string& name) {
		auto g = std::make_unique<BlueprintGraph>();
		g->name = name;
		BlueprintGraph* ptr = g.get();
		graphs.push_back(std::move(g));
		return ptr;
	}

	bool CompileGraph(BlueprintGraph& graph) { return compiler.Compile(graph); }

	BlueprintGraph* FindGraph(const std::string& name) {
		for (auto& g : graphs) if (g->name == name) return g.get();
		return nullptr;
	}
};

} // namespace arclight
