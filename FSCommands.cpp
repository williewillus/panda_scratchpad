#include "FSCommands.h"

using std::string;

namespace {
	string mkfsCommand;
	string mountCommand;
}



class DAXCommands : public FSCommands {

      	public:
	static constexpr const char fsName[] = "dax";

	virtual string GetFSName() {
		return string(fsName);
	}
	
	// FOr filesystems like PMFS and Nova, the mkfs command mounts the FS as well
	// So to maintain the same everywhere, we will mount DAX after creating the FS
	virtual string GetMkfsCommand(string &dev_path, string &mount_point) {
		mkfsCommand = "mkfs.ext4 -b 4096 " + dev_path;
		mkfsCommand += "; " + GetMountCommand(dev_path, mount_point);
		return mkfsCommand;
	}

	virtual string GetMountCommand(string &dev_path, string &mount_point) {
		mountCommand = "mount -t ext4 -o dax " + dev_path + " " + mount_point ;
		return mountCommand;
	}
};


class NOVACommands : public FSCommands {
	public:
	static constexpr const char fsName[] = "NOVA";

	virtual string GetFSName() {
		return string(fsName);
	}

	virtual string GetMkfsCommand(string &dev_path, string &mount_point) {
		mkfsCommand = "mount -t " + string(fsName) + " -o init " + dev_path + " " + mount_point;
		return mkfsCommand;
	}

	virtual string GetMountCommand(string &dev_path, string &mount_point) {
		mountCommand = "mount -t " + string(fsName) + " " + dev_path + " " + mount_point ;
		return mountCommand;
	}
};



class PMFSCommands : public FSCommands {
	public:
	static constexpr const char fsName[] = "pmfs";

	virtual string GetFSName() {
		return string(fsName);
	}

	virtual string GetMkfsCommand(string &dev_path, string &mount_point) {
		mkfsCommand = "mount -t " + string(fsName) + " -o init " + dev_path + " " + mount_point;
		return mkfsCommand;
	}

	virtual string GetMountCommand(string &dev_path, string &mount_point) {
		mountCommand = "mount -t " + string(fsName) + " " + dev_path + " " + mount_point ;
		return mountCommand;
	}
};



constexpr const char DAXCommands::fsName[];
constexpr const char NOVACommands::fsName[];
constexpr const char PMFSCommands::fsName[];

FSCommands* GetFSCommands(string &fs_name){
	if (fs_name.compare(DAXCommands::fsName) == 0 )
		return new DAXCommands(); 
	else if (fs_name.compare(NOVACommands::fsName) == 0 )
		return new NOVACommands();
	else if (fs_name.compare(PMFSCommands::fsName) == 0 )
		return new PMFSCommands();
	else
		return NULL;
}
