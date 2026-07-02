/* ArcLight Engine - Blueprint System (UE5-inspired)
 * Developer: Stephen
 * Visual scripting system with nodes, connections, execution flow,
 * variables, events, macros, and compilation to bytecode.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstdint>

namespace arclight {

// ======================== Blueprint Types ========================

enum class EBlueprintNodeType {
	Entry,
	Event,
	Function,
	Variable,
	Math,
	Branch,
	ForEach,
	Print,
	SetVariable,
	GetVariable,
	Custom,
};

enum class EPinDirection {
	Input,
	Output,
};

enum class EPinType {
	Bool,
	Int,
	Float,
	String,
	Vector,
	Object,
	Exec, // execution flow
	Unknown,
};

struct BlueprintPin {
	std::string id;
	std::string name;
	EPinDirection direction = EPinDirection::Input;
	EPinType type = EPinType::Unknown;
	bool isLinked = false;
	std::string linkedPinID;
	float3 position = ZeroVector;
};

struct BlueprintNode {
	std::string id;
	std::string name;
	std::string description;
	EBlueprintNodeType type = EBlueprintNodeType::Custom;
	float3 position = ZeroVector;
	float2 size = float2(200, 80);
	std::vector<BlueprintPin> inputPins;
	std::vector<BlueprintPin> outputPins;
	bool isEnabled = true;
	bool isBreakpoint = false;

	void AddInputPin(const std::string& pinName, EPinType pinType) {
		BlueprintPin pin;
		pin.id = id + "_in_" + std::to_string(inputPins.size());
		pin.name = pinName;
		pin.direction = EPinDirection::Input;
		pin.type = pinType;
		inputPins.push_back(pin);
	}

	void AddOutputPin(const std::string& pinName, EPinType pinType) {
		BlueprintPin pin;
		pin.id = id + "_out_" + std::to_string(outputPins.size());
		pin.name = pinName;
		pin.direction = EPinDirection::Output;
		pin.type = pinType;
		outputPins.push_back(pin);
	}

	void AddExecPin(const std::string& pinName) {
		BlueprintPin pin;
		pin.id = id + "_exec_" + std::to_string(outputPins.size());
		pin.name = pinName;
		pin.direction = EPinDirection::Output;
		pin.type = EPinType::Exec;
		outputPins.push_back(pin);
	}

	BlueprintPin* GetPin(const std::string& pinID) {
		for (auto& p : inputPins) { if (p.id == pinID) return &p; }
		for (auto& p : outputPins) { if (p.id == pinID) return &p; }
		return nullptr;
	}
};

// ======================== Blueprint Variable ========================

struct BlueprintVariable {
	std::string name;
	EPinType type = EPinType::Unknown;
	std::string defaultValue;
	bool isExposed = false;
	bool isReadonly = false;
};

// ======================== Blueprint Connection ========================

struct BlueprintConnection {
	std::string id;
	std::string fromNodeID;
	std::string fromPinID;
	std::string toNodeID;
	std::string toPinID;
	bool isEnabled = true;
};

// ======================== Blueprint Graph ========================

class BlueprintGraph {
public:
	std::string name;
	std::vector<std::unique_ptr<BlueprintNode>> nodes;
	std::vector<BlueprintConnection> connections;
	std::vector<BlueprintVariable> variables;
	std::string ownerEntityID;

	uint32_t nextNodeID = 1;
	uint32_t nextConnID = 1;

	BlueprintNode* AddNode(EBlueprintNodeType nodeType, const std::string& nodeName, float x, float y) {
		auto node = std::make_unique<BlueprintNode>();
		node->id = "node_" + std::to_string(nextNodeID++);
		node->name = nodeName;
		node->type = nodeType;
		node->position = float3(x, y, 0);

		switch (nodeType) {
			case EBlueprintNodeType::Entry:
				node->AddOutputPin("Exec", EPinType::Exec);
				break;
			case EBlueprintNodeType::Event:
				node->AddOutputPin("Exec", EPinType::Exec);
				node->AddOutputPin("Self", EPinType::Object);
				break;
			case EBlueprintNodeType::Branch:
				node->AddInputPin("Exec", EPinType::Exec);
				node->AddInputPin("Condition", EPinType::Bool);
				node->AddOutputPin("True", EPinType::Exec);
				node->AddOutputPin("False", EPinType::Exec);
				break;
			case EBlueprintNodeType::Print:
				node->AddInputPin("Exec", EPinType::Exec);
				node->AddInputPin("Text", EPinType::String);
				node->AddOutputPin("Exec", EPinType::Exec);
				break;
			case EBlueprintNodeType::Math:
				node->AddInputPin("A", EPinType::Float);
				node->AddInputPin("B", EPinType::Float);
				node->AddOutputPin("Result", EPinType::Float);
				break;
			case EBlueprintNodeType::SetVariable:
				node->AddInputPin("Exec", EPinType::Exec);
				node->AddInputPin("Value", EPinType::Unknown);
				node->AddOutputPin("Exec", EPinType::Exec);
				break;
			case EBlueprintNodeType::GetVariable:
				node->AddOutputPin("Value", EPinType::Unknown);
				break;
			case EBlueprintNodeType::ForEach:
				node->AddInputPin("Exec", EPinType::Exec);
				node->AddInputPin("Array", EPinType::Unknown);
				node->AddOutputPin("Loop Body", EPinType::Exec);
				node->AddOutputPin("Element", EPinType::Unknown);
				node->AddOutputPin("Index", EPinType::Int);
				node->AddOutputPin("Completed", EPinType::Exec);
				break;
			default:
				break;
		}

		BlueprintNode* ptr = node.get();
		nodes.push_back(std::move(node));
		return ptr;
	}

	BlueprintConnection* Connect(const std::string& fromNode, const std::string& fromPin,
		const std::string& toNode, const std::string& toPin) {
		BlueprintConnection conn;
		conn.id = "conn_" + std::to_string(nextConnID++);
		conn.fromNodeID = fromNode;
		conn.fromPinID = fromPin;
		conn.toNodeID = toNode;
		conn.toPinID = toPin;
		connections.push_back(conn);
		return &connections.back();
	}

	void Disconnect(const std::string& connID) {
		connections.erase(
			std::remove_if(connections.begin(), connections.end(),
				[&](const BlueprintConnection& c) { return c.id == connID; }),
			connections.end()
		);
	}

	void RemoveNode(const std::string& nodeID) {
		nodes.erase(
			std::remove_if(nodes.begin(), nodes.end(),
				[&](const std::unique_ptr<BlueprintNode>& n) { return n->id == nodeID; }),
			nodes.end()
		);
		connections.erase(
			std::remove_if(connections.begin(), connections.end(),
				[&](const BlueprintConnection& c) { return c.fromNodeID == nodeID || c.toNodeID == nodeID; }),
			connections.end()
		);
	}

	void AddVariable(const BlueprintVariable& var) { variables.push_back(var); }

	BlueprintNode* FindNode(const std::string& nodeID) {
		for (auto& n : nodes) { if (n->id == nodeID) return n.get(); }
		return nullptr;
	}

	bool IsValid() const {
		for (auto& n : nodes) {
			if (n->type == EBlueprintNodeType::Entry) return true;
		}
		return false;
	}
};

// ======================== Blueprint Compiler ========================

struct BytecodeInstruction {
	uint8_t opcode = 0;
	uint32_t operandA = 0;
	uint32_t operandB = 0;
	uint32_t operandC = 0;
};

class BlueprintCompiler {
public:
	std::vector<BytecodeInstruction> bytecode;
	std::string errorMessage;
	bool compilationSuccess = false;

	bool Compile(BlueprintGraph& graph) {
		bytecode.clear();
		errorMessage.clear();
		compilationSuccess = false;

		if (!graph.IsValid()) {
			errorMessage = "Blueprint has no entry node";
			return false;
		}

		// Find entry node and trace execution flow
		for (auto& node : graph.nodes) {
			if (node->type == EBlueprintNodeType::Entry) {
				CompileNode(graph, node.get());
			}
		}

		compilationSuccess = true;
		return true;
	}

	void CompileNode(BlueprintGraph& graph, BlueprintNode* node) {
		if (!node || !node->isEnabled) return;

		BytecodeInstruction instr;
		instr.opcode = static_cast<uint8_t>(node->type);

		switch (node->type) {
			case EBlueprintNodeType::Entry:
				instr.opcode = 0x01; // OP_ENTRY
				break;
			case EBlueprintNodeType::Branch:
				instr.opcode = 0x10; // OP_BRANCH
				break;
			case EBlueprintNodeType::Print:
				instr.opcode = 0x20; // OP_PRINT
				break;
			case EBlueprintNodeType::SetVariable:
				instr.opcode = 0x30; // OP_SET_VAR
				break;
			case EBlueprintNodeType::GetVariable:
				instr.opcode = 0x31; // OP_GET_VAR
				break;
			case EBlueprintNodeType::Math:
				instr.opcode = 0x40; // OP_MATH
				break;
			default:
				instr.opcode = 0xFF; // OP_NOP
				break;
		}
		bytecode.push_back(instr);

		// Follow execution pins to next nodes
		for (auto& conn : graph.connections) {
			if (conn.fromNodeID == node->id && conn.isEnabled) {
				BlueprintNode* next = graph.FindNode(conn.toNodeID);
				if (next && next->type != EBlueprintNodeType::Branch) {
					CompileNode(graph, next);
				}
			}
		}
	}

	size_t GetBytecodeSize() const { return bytecode.size(); }
};

// ======================== Blueprint Runtime ========================

class BlueprintRuntime {
public:
	std::vector<BytecodeInstruction> program;
	size_t programCounter = 0;
	bool isRunning = false;
	std::unordered_map<std::string, std::string> variables;

	void Load(const std::vector<BytecodeInstruction>& code) {
		program = code;
		programCounter = 0;
		variables.clear();
	}

	void Execute(BlueprintGraph& graph) {
		isRunning = true;
		programCounter = 0;

		while (programCounter < program.size() && isRunning) {
			auto& instr = program[programCounter];
			ExecuteInstruction(graph, instr);
			programCounter++;
		}
		isRunning = false;
	}

	void Stop() { isRunning = false; }

	void SetVariable(const std::string& name, const std::string& value) {
		variables[name] = value;
	}

	std::string GetVariable(const std::string& name) const {
		auto it = variables.find(name);
		return (it != variables.end()) ? it->second : "";
	}

	std::function<void(const std::string&)> onPrint;
	std::function<void(const std::string&, const std::string&)> onVariableChanged;

private:
	void ExecuteInstruction(BlueprintGraph& /*graph*/, const BytecodeInstruction& instr) {
		switch (instr.opcode) {
			case 0x01: // OP_ENTRY
				break;
			case 0x10: // OP_BRANCH
				break;
			case 0x20: // OP_PRINT
				if (onPrint) onPrint("Blueprint output");
				break;
			case 0x30: // OP_SET_VAR
				break;
			case 0x31: // OP_GET_VAR
				break;
			default:
				break;
		}
	}
};

// ======================== Main Blueprint System ========================

class BlueprintSystem {
public:
	std::vector<std::unique_ptr<BlueprintGraph>> graphs;
	BlueprintCompiler compiler;
	BlueprintRuntime runtime;
	uint32_t nextGraphID = 1;

	BlueprintGraph* CreateGraph(const std::string& name) {
		auto graph = std::make_unique<BlueprintGraph>();
		graph->name = name;
		BlueprintGraph* ptr = graph.get();
		graphs.push_back(std::move(graph));
		return ptr;
	}

	bool CompileGraph(BlueprintGraph& graph) {
		return compiler.Compile(graph);
	}

	void ExecuteGraph(BlueprintGraph& graph) {
		if (CompileGraph(graph)) {
			runtime.Load(compiler.bytecode);
			runtime.Execute(graph);
		}
	}

	BlueprintGraph* FindGraph(const std::string& name) {
		for (auto& g : graphs) { if (g->name == name) return g.get(); }
		return nullptr;
	}

	void RemoveGraph(const std::string& name) {
		graphs.erase(
			std::remove_if(graphs.begin(), graphs.end(),
				[&](const std::unique_ptr<BlueprintGraph>& g) { return g->name == name; }),
			graphs.end()
		);
	}
};

} // namespace arclight
