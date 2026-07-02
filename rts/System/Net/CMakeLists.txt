add_library(engineSystemNet STATIC EXCLUDE_FROM_ALL
	LocalConnection.cpp
	LoopbackConnection.cpp
	PackPacket.cpp
	ProtocolDef.cpp
	RawPacket.cpp
	Socket.cpp
	UDPConnection.cpp
	UDPListener.cpp
	UnpackPacket.cpp
)

target_include_directories(engineSystemNet
	PRIVATE
		${CMAKE_SOURCE_DIR}/rts
	PUBLIC
		${CMAKE_SOURCE_DIR}/rts/lib/asio/include
)

