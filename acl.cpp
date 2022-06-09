#include <iostream>
#include <map>


class MyUser{
	public:
		std::string name;
		std::string group;

		MyUser(std::string aName, std::string aGroup){
			name = aName;
			group = aGroup;
		}

};

class File{
	public:
		std::string fileName;
		std::string fileContents;
		std::string whenExecuted;
		std::string owner;
		std::string group;
		int ownerPerms;
		int groupPerms;
		int inum;

		File(std::string aFileName, std::string aFileContents,
			std::string aWhenExecuted, 
			std::string aFileOwner,
			std::string aGroup,
			int aOwnerPerms,
			int aGroupPerms,
			int aInum){
			fileName = aFileName;
			fileContents = aFileContents;
			whenExecuted = aWhenExecuted;
			owner = aFileOwner;
			group = aGroup;
			ownerPerms = aOwnerPerms;
			groupPerms = aGroupPerms;
			inum = aInum;
		}
	
		// Read method
		void readIt(File &f){
			std::cout << fileContents << std::endl;
		}

		// Execute file
		void executeIt(){
			std::cout << whenExecuted << std::endl;
		}

		// Write to file
		void writeIt(std::string newContents, File &f){
			f.fileContents = newContents;
			std::cout << "Wrote to file: " << f.fileContents << std::endl;
		}
};


class Kernel{
	public:
		std::map<int, std::map<std::string, int>> acl = {{{}, {{},}}};
		Kernel(){
			acl = {{{}, {{}, }}};
		}

		// Allow for updating the acl
		void updateACL(File f, MyUser u, int perms){
			acl[f.inum][u.name] = perms;
		}

		// For checking read perms granted via the ACL
		bool checkRead(File &f, MyUser u){
			auto it = acl.find(f.inum);
			if (it == acl.end()){
				return false;
			}

			else{
				std::map <std::string, int> filePerms = it -> second;
				auto it_two = (filePerms).find(u.name);
				if(it_two == filePerms.end()){
					return false;
				}

				else{
					int u_perms = it_two -> second;
					if(u_perms >= 4){
						return true;
					}
				}
			}
			return false;
		}

		// For checking execute perms granted via the ACL
		bool checkExec(File &f, MyUser u){
			auto it = acl.find(f.inum);
			if (it == acl.end()){
				return false;
			}

			else{
				std::map <std::string, int> filePerms = it -> second;
				auto it_two = (filePerms).find(u.name);
				if(it_two == filePerms.end()){
					return false;
				}

				else{
					int u_perms = it_two -> second;
					if(u_perms % 2 == 1){
						return true;
					}
				}
			}

			return false;
		}

		// For checking write perms granted via the ACL
		bool checkWrite(File &f, MyUser u){
			auto it = acl.find(f.inum);
			if (it == acl.end()){
				return false;
			}

			else{
				std::map <std::string, int> filePerms = it -> second;
				auto it_two = (filePerms).find(u.name);
				if(it_two == filePerms.end()){
					return false;
				}

				else{
					int u_perms = it_two -> second;
					if(u_perms == 2 || u_perms == 3 || u_perms == 6 || u_perms == 7){
						return true;
					}
				}
			}

			return false;
		}

		// Read method
		void readFile(MyUser u, File &f){
			if(u.name == f.owner && f.ownerPerms >= 4){
				f.readIt(f);
			}

			else if(u.group == f.group && f.groupPerms >= 4){
				f.readIt(f);
			}

			else{
				// Check ACL
				if (checkRead(f, u)){
					f.readIt(f);
				}

				else{
					std::cout << "Permission denied!"  << std::endl;
				}
			}
		}

		// Execute file
		void executeFile(MyUser u, File &f){
			if(u.name == f.owner && f.ownerPerms % 2 == 1){
				f.executeIt();
			}

			else if(u.group == f.group && f.groupPerms % 2 == 1){
				f.executeIt();
			}

			else{
				// Check ACL
				if (checkExec(f, u)){
					f.executeIt();
				}

				else{
					std::cout<< "Permission denied!" << std::endl;
				}
			}
		}

		// Write to file
		void writeFile(MyUser u, File &f, std::string newContents){
			if(u.name == f.owner && (f.ownerPerms == 2 || f.ownerPerms == 3 ||
					f.ownerPerms == 6 || f.ownerPerms == 7)){
				f.writeIt(newContents, f);
			}

			else if(u.group == f.group && (f.groupPerms == 2 || f.groupPerms == 3 ||
					f.groupPerms == 6 || f.groupPerms == 7)){
				f.writeIt(newContents, f);
			}

			else{
				// Check ACL
				if (checkWrite(f, u)){
					f.writeIt(newContents, f);
				}

				else{
					std::cout<< "Permission denied!" << std::endl;
				}
			}
		}
};

int main(){

	//Initalize the file
	std::string myFileName = "helloworld.exe";
	std::string myFileContents = "Hello, World!";
	std::string myWhenExecuted = "You executed hello world!";
	std::string myFileOwner = "Bob";
	std::string myFileGroup = "Accounting";
	int ownerPerms = 7;
	int groupPerms = 5;
	int myInum = 177;
	File myFile = File(myFileName, myFileContents, myWhenExecuted, myFileOwner, myFileGroup, ownerPerms, groupPerms, myInum);

	MyUser bob = MyUser("Bob", "Accounting");
	MyUser joe = MyUser("Joe", "Accounting");
	MyUser sally = MyUser("Sally", "Marketing");

	// File perms
	// 0 --> No perms
	// 1 --> Execute 
	// 2 --> Write
	// 3 --> Execute and Write
	// 4 --> Read
	// 5 --> Read and Execute
	// 6 --> Read and Write
	// 7 --> All perms

	// Init ACL
	std::map<int, std::map<std::string, int>> newFs = {{{}, {{},}}};

	
	// Init the kernel
	Kernel k = Kernel();

	//Bob should be able to read, write, and exeucte he owns the file and has read perms
	std::cout << "======= Bob =======" << std::endl;
	k.readFile(bob, myFile);
	k.executeFile(bob, myFile);
	k.writeFile(bob, myFile, "Bob wrote this.");

	// Even though Joe doesn't own the file, he is the group which owns the file, and the group can read and execute
	std::cout << "\n======= Joe =======" << std::endl;
	k.readFile(joe, myFile);
	k.executeFile(joe, myFile);
	k.writeFile(joe, myFile, "Joe wrote this.");

	// Sally should not be able to read the file
	std::cout << "\n======= Sally =======" << std::endl;
	k.readFile(sally, myFile);
	k.executeFile(sally, myFile);
	k.writeFile(sally, myFile, "Sally wrote this.");
	
	// Update the ACL for sally
	k.updateACL(myFile, sally, 4);
	std::cout << "\n======= Sally Read ACL =======" << std::endl;
	k.readFile(sally, myFile);
	k.executeFile(sally, myFile);
	k.writeFile(sally, myFile, "Sally wrote this.");

	// Update the ACL for sally
	k.updateACL(myFile, sally, 1);
	std::cout << "\n======= Sally Execute ACL =======" << std::endl;
	k.readFile(sally, myFile);
	k.executeFile(sally, myFile);
	k.writeFile(sally, myFile, "Sally wrote this.");

	// Update the ACL for sally
	k.updateACL(myFile, sally, 2);
	std::cout << "\n======= Sally Write ACL =======" << std::endl;
	k.readFile(sally, myFile);
	k.executeFile(sally, myFile);
	k.writeFile(sally, myFile, "Sally wrote this.");

	// Update the ACL for sally
	k.updateACL(myFile, sally, 7);
	std::cout << "\n======= Sally All Perms  ACL =======" << std::endl;
	k.readFile(sally, myFile);
	k.executeFile(sally, myFile);
	k.writeFile(sally, myFile, "Sally wrote this.");

	return 0;
}
