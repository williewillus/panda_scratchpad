#ifndef QEMU_SOCKET_UTILS_H
#define QEMU_SOCKET_UTILS_H

namespace communication {

// The client/guest VM would send Qemu monitor messages to the
// Qemu monitor server that is already listening.
// The response must be string values

typedef enum {
	cListPlugins,
	cLoadPlugin
} QemuCommand;

typedef enum {
	// Error in connection
	eNotConnected,
	eAlreadyConnected,
	eOther,
	eNone
} SockError;

struct SockMessage {
	QemuCommand q_cmd;
	bool need_response;
	std::string cmd_options;
};

} // namespace communication

#endif // QEMU_SOCKET_UTILS_H


