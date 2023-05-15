#ifndef ADDRESSMANAGER_H
#define ADDRESSMANAGER_H

#include <string>
#include <vector>

using namespace std;


typedef vector<string>		ADDRESS_LIST;
typedef ADDRESS_LIST::iterator	ADDRESS_IT;

class AddressManager
{
	ADDRESS_LIST _addresses;
public:
	AddressManager();
	~AddressManager();

	void setPath(string path);
	string getPath(void);
	void resetVector(void);
	bool parseXmlToList(void);
	bool addAddress(string mac);
	bool saveAsFile(void);
	int getItemCount(void);

protected:



private:
	string path;

	void clear();
};

#endif // ADDRESSMANAGER_H
