#ifndef FS_COMMANDS_H
#define FS_COMMANDS_H

#include <iostream>
#include <string>

class FSCommands {
	public:
		// Returns the filesystem name	
		virtual std::string GetFSName() = 0;

		// Returns the FS specific mkfs command.
		virtual std::string GetMkfsCommand(std::string &device_path, std::string &mount_point) = 0;

		// Return sthe mount command specific to the FS
		virtual std::string GetMountCommand(std::string &device_path, std::string &mount_point) = 0;

};

FSCommands* GetFSCommands(std::string &fs_name);

#endif // _FS_COMMANDS_H

