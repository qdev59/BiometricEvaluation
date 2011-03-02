/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <stdlib.h>
#include <string.h>

#ifdef FILERECORDSTORETEST
#include <be_io_filerecstore.h>
#define TESTDEFINED
#endif

#ifdef DBRECORDSTORETEST
#include <be_io_dbrecstore.h>
#define TESTDEFINED
#endif

#ifdef ARCHIVERECORDSTORETEST
#include <be_io_archiverecstore.h>
#define TESTDEFINED
#endif

#ifdef TESTDEFINED
using namespace BiometricEvaluation;
#endif

static const int SEQUENCECOUNT = 10;
static const int RDATASIZE = 64;
static string rsname;

/*
 * Test the ability to sequence through the entire RecordStore.
 */
static void
testSequence(IO::RecordStore *rs)
{
	char rdata[RDATASIZE];
	string theKey;
	uint64_t rlen;

	try {
		int i = 1;
		while (true) {
			try {
				rlen = rs->sequence(theKey, rdata);
				cout << "Record " << i << " key is " << theKey;
				cout << "; record length is " << rlen << "; ";
				printf("data is [%s]\n", rdata);
			} catch (Error::ObjectDoesNotExist &e) {
				break;
			} catch (Error::StrategyError &e) {
				cout << "Caught " << e.getInfo() << endl;
			}
			i++;
		}
	} catch (Error::StrategyError &e) {
		cout << "Caught " << e.getInfo() << endl;
	}
}

/*
 * Test the ability to merge RecordStores of different types
 */
static void
testMerge()
{
	string merged_type;
	const size_t num_rs = 3;
	const string merge_rs_fn[num_rs] = {"merge_test1", "merge_test2", 
	    "merge_test3"};
	IO::RecordStore *merged_rs;
	IO::RecordStore *merge_rs[num_rs];

	try {
#ifdef ARCHIVERECORDSTORETEST
		merged_type = IO::RecordStore::ARCHIVETYPE;
		merge_rs[0] = new IO::ArchiveRecordStore(merge_rs_fn[0],
		    "RS for merge", "");
		merge_rs[1] = new IO::ArchiveRecordStore(merge_rs_fn[1],
		    "RS for merge", "");
		merge_rs[2] = new IO::ArchiveRecordStore(merge_rs_fn[2],
		    "RS for merge", "");
#endif
#ifdef DBRECORDSTORETEST
		merged_type = IO::RecordStore::BERKELEYDBTYPE;
		merge_rs[0] = new IO::DBRecordStore(merge_rs_fn[0],
		    "RS for merge", "");
		merge_rs[1] = new IO::DBRecordStore(merge_rs_fn[1],
		    "RS for merge", "");
		merge_rs[2] = new IO::DBRecordStore(merge_rs_fn[2],
		    "RS for merge", "");
#endif
#ifdef FILERECORDSTORETEST
		merged_type = IO::RecordStore::FILETYPE;
		merge_rs[0] = new IO::FileRecordStore(merge_rs_fn[0],
		    "RS for merge", "");
		merge_rs[1] = new IO::FileRecordStore(merge_rs_fn[1],
		    "RS for merge", "");
		merge_rs[2] = new IO::FileRecordStore(merge_rs_fn[2],
		    "RS for merge", "");
#endif
		merge_rs[0]->insert("0", "0", 2);
		merge_rs[0]->insert("1", "1", 2);
		merge_rs[0]->insert("2", "2", 2);
		merge_rs[1]->insert("3", "3", 2);
		merge_rs[1]->insert("4", "4", 2);
		merge_rs[1]->insert("5", "5", 2);
		merge_rs[2]->insert("6", "6", 2);
		merge_rs[2]->insert("7", "7", 2);
		merge_rs[2]->insert("8", "8", 2);

		const string merged_rs_fn = "test_merged";
		IO::RecordStore::mergeRecordStores(merged_rs_fn,
		    "A merge of 3 RS", "", merged_type, merge_rs, num_rs);
#ifdef ARCHIVERECORDSTORETEST
		merged_rs = new IO::ArchiveRecordStore(merged_rs_fn, "");
#endif
#ifdef DBRECORDSTORETEST
		merged_rs = new IO::DBRecordStore(merged_rs_fn, "");
#endif
#ifdef FILERECORDSTORETEST
		merged_rs = new IO::FileRecordStore(merged_rs_fn, "");
#endif
		if (merged_rs->getCount() == (num_rs * 3))
			cout << "success." << endl;
		else
			cout << "FAILED." << endl;

		if (merged_rs != NULL) {
			delete merged_rs; 
			IO::RecordStore::removeRecordStore(merged_rs_fn, "");
		}
		if (merge_rs[0] != NULL) {
			delete merge_rs[0];
			IO::RecordStore::removeRecordStore(merge_rs_fn[0], "");
		}
		if (merge_rs[1] != NULL) {
			delete merge_rs[1];
			IO::RecordStore::removeRecordStore(merge_rs_fn[1], "");
		}
		if (merge_rs[2] != NULL) {
			delete merge_rs[2];
			IO::RecordStore::removeRecordStore(merge_rs_fn[2], "");
		}
	} catch (Error::Exception &e) {
		cout << "Caught " << e.getInfo() << endl;
	}
}

/*
 * Test the read and write operations of a RecordStore. This function will
 * test any implementation of the abstract RecordStore by using the abstract
 * RecordStore interface.
 */
int
runTests(IO::RecordStore *rs)
{
	/*
	 * Insert a record to the RecordStore so we can read/write it.
	 */
	string theKey("firstRec");
	string wdata = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint64_t rlen;
	uint64_t wlen = wdata.size();
	try {
		cout << "insert(" << theKey << "): ";
		rs->insert(theKey, wdata.c_str(), wlen);
	} catch (Error::ObjectExists& e) {
		cout << "exists; deleting." << endl;
		try {
			rs->remove(theKey);
			rs->insert(theKey, wdata.c_str(), wlen);
		} catch (Error::StrategyError& e) {
			cout << "Could not remove, and should be able to: " <<
			    e.getInfo() << "." << endl;
			return (-1);
		}
	} catch (Error::StrategyError& e) {
		cout << "failed:" << e.getInfo() << "." << endl;
		return (-1);
	}
	cout << endl;
	cout << "Count of records is " << rs->getCount() << endl;

	char rdata[RDATASIZE];
	bzero(rdata, RDATASIZE);
	try {
		cout << "read(" << theKey << "): ";
		rlen = rs->read(theKey, rdata);
	} catch (Error::ObjectDoesNotExist& e) {
		cout << "failed: Does not exist. " << endl;
		return (-1);
	} catch (Error::StrategyError& e) {
		cout << "failed: " << e.getInfo() << "." << endl;
		return (-1);
	}
	cout << "succeeded, read [" << rdata << "] ";
	if (rlen != wlen)
		cout << "failed: length of " << rlen << " is incorrect." << endl;
	else
		cout << "and length is correct." << endl;

	wdata = "ZYXWVUTSRQPONMLKJIHGFEDCBA0123456789";
	wlen = wdata.size();
	try {
		cout << "replace(" << theKey << "): ";
		rs->replace(theKey, wdata.c_str(), wlen);
	} catch (Error::ObjectDoesNotExist& e) {
		cout << "does not exist!" << endl;
		return (-1);
	} catch (Error::StrategyError& e) {
		cout << "failed:" << e.getInfo() << "." << endl;
		return (-1);
	}
	cout << endl;
	cout << "Count of records is " << rs->getCount() << endl;
	cout << "Space usage is ";
	try {
		cout << rs->getSpaceUsed() << endl;
	} catch (Error::StrategyError& e) {
		cout << "failed:" << e.getInfo() << "." << endl;
	}

	bzero(rdata, RDATASIZE);
	rlen = rs->read(theKey, rdata);
	cout << "Second read yields [" << rdata << "]" << endl;

	try {
		cout << "length(" << theKey << "): ";
		rlen = rs->length(theKey);
	} catch (Error::ObjectDoesNotExist& e) {
		cout << "does not exist!" << endl;
		return (-1);
	} catch (Error::StrategyError& e) {
		cout << "failed:" << e.getInfo() << "." << endl;
		return (-1);
	}
	if (rlen != wlen) {
		cout << "failed: length " << rlen << " is incorrect." << endl;
		return (-1);
	} else {
		cout << rlen << " is correct." << endl;
	}

	cout << "Deleting record... ";
	rs->remove(theKey);
	cout << "Record count is now " << rs->getCount() << endl;

	/*
	 * Try to read the record we just deleted.
	 */
	bzero(rdata, RDATASIZE);
	bool success;
	try {
		cout << "Non-existent read(" << theKey << "): ";
		rlen = rs->read(theKey, rdata);
		success = false;
	} catch (Error::ObjectDoesNotExist& e) {
		cout << "succeeded." << endl;
		success = true;
	} catch (Error::StrategyError& e) {
		cout << "failed: " << e.getInfo() << "." << endl;
		return (-1);
	}
	if (!success)
		cout << "failed." << endl;

	try {
		cout << "sync(): ";
		rs->sync();
		cout << "succeeded." << endl;
	} catch (Error::StrategyError& e) {
		cout << "failed: " << e.getInfo() << "." << endl;
		return (-1);
	}
	cout << "Space usage with no records is " << rs->getSpaceUsed() << endl;
	cout << "Sequencing records..." << endl;
	int i;
	for (i = 0; i < SEQUENCECOUNT; i++) {
		bzero(rdata, RDATASIZE);
		snprintf(rdata, RDATASIZE, "key%u", i);
		theKey.assign(rdata);
		snprintf(rdata, RDATASIZE, "Bogus data for key%u", i);
		rs->insert(theKey, rdata, RDATASIZE);
	}
	testSequence(rs);
	theKey.assign("key5");
	cout << "Sequence, starting from \"" << theKey << "\"" << endl;
	try {
		rs->setCursor(theKey);
	} catch (Error::Exception &e) {
		cout << "Caught: " << e.getInfo() << endl;
	}
	testSequence(rs);
	cout << "Changing RecordStore name..." << endl;
	try {
		string newName = tempnam(".", NULL);
		newName = newName.substr(2, newName.length());
		rs->changeName(newName);
	} catch (Error::ObjectExists &e) {
		cout << "failed: " << e.getInfo() << "." << endl;
		return (-1);
	} catch (Error::StrategyError &e) {
		cout << "failed: " << e.getInfo() << "." << endl;
		return (-1);
	}
	cout << "Name is now " << rs->getName() << endl;
	cout << "Deleting all records..." << endl;
	for (i = 0; i < SEQUENCECOUNT; i++) {
		snprintf(rdata, RDATASIZE, "key%u", i);
		theKey.assign(rdata);
		try { 
			rs->remove(theKey);
		} catch (Error::StrategyError &e) {
			cout << "Caught: " << e.getInfo() << endl;
		}
	}
	cout << "Sequencing empty store... ";
	testSequence(rs);
	cout << "there should be no output." << endl;

#ifdef FILERECORDSTORETEST
	cout << "Insert with an invalid key into FileRecordStore..." << endl;
	try {
		string badKey("test/with/path/chars");
		rs->insert(badKey, rdata, RDATASIZE);
	} catch (Error::ObjectExists &e) {
		cout << "Caught: " << e.getInfo() << endl;
	} catch (Error::StrategyError &e)  {
		cout << "Caught: " << e.getInfo() << endl;
		cout << "\tShould be invalid key." << endl;
	}
#endif

	cout << "Return RecordStore to original name... ";
	try {
		rs->changeName(rsname);
	} catch (Error::ObjectExists &e) {
		cout << "Caught: " << e.getInfo();
	} catch (Error::StrategyError &e)  {
		cout << "Caught: " << e.getInfo();
	}
	cout << "finished." << endl;

	return (0);
}

int
main(int argc, char* argv[]) {

	/*
	 * Other types of RecordStore objects can be created here and
	 * accessed via the RecordStore interface.
	 */

#ifdef FILERECORDSTORETEST

	/* Call the constructor that will create a new FileRecordStore. */
	rsname = "frs_test";
	IO::FileRecordStore *rs;
	try {
		rs = new IO::FileRecordStore(rsname, "RW Test Dir", "");
	} catch (Error::ObjectExists& e) {
		cout << "The File Record Store exists; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (Error::StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
		return (EXIT_FAILURE);
	}
	auto_ptr<IO::FileRecordStore> ars(rs);
#endif

#ifdef DBRECORDSTORETEST
	/* Call the constructor that will create a new DBRecordStore. */
	rsname = "dbrs_test";
	IO::DBRecordStore *rs;
	try {
		rs = new IO::DBRecordStore(rsname, "RW Test Dir", "");
	} catch (Error::ObjectExists &e) {
		cout << "The DB Record Store exists; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (Error::StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
		return (EXIT_FAILURE);
	}
	auto_ptr<IO::DBRecordStore> ars(rs);
#endif

#ifdef ARCHIVERECORDSTORETEST
	/* Call the constructor that will create a new ArchiveRecordStore. */
	rsname = "ars_test";
	IO::ArchiveRecordStore *rs;
	try {
		rs = new IO::ArchiveRecordStore(rsname, "RW Test Dir", "");
	} catch (Error::ObjectExists &e) {
		cout << "The Archive Record Store exists; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (Error::StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
		return (EXIT_FAILURE);
	}
	auto_ptr<IO::ArchiveRecordStore> ars(rs);
#endif
#ifdef TESTDEFINED

	cout << "Running tests with new record store:" << endl;
	if (runTests(rs) != 0)
		return (EXIT_FAILURE);
#endif

#ifdef FILERECORDSTORETEST

	/* Call the constructor that will open an existing FileRecordStore. */
	rsname = "frs_test";
	try {
		rs = new IO::FileRecordStore(rsname, "");
	} catch (Error::ObjectDoesNotExist& e) {
		cout << "The File Record Store does not exist; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (Error::StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
		return (EXIT_FAILURE);
	}
#endif

#ifdef DBRECORDSTORETEST
	/* Call the constructor that will open an existing DBRecordStore. */
	rsname = "dbrs_test";
	try {
		rs = new IO::DBRecordStore(rsname, "");
	} catch (Error::ObjectDoesNotExist &e) {
		cout << "The DB Record Store does not exist; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (Error::StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
		return (EXIT_FAILURE);
	}
#endif

#ifdef ARCHIVEECORDSTORETEST
	/* Call the constructor that will open an existing ArchiveRecordStore.*/
	rsname = "ars_test";
	try {
		rs = new IO::ArchiveRecordStore(rsname, "");
	} catch (Error::ObjectDoesNotExist &e) {
		cout << "The Archive Record Store does not exist; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (Error::StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
		return (EXIT_FAILURE);
	}
#endif

#ifdef TESTDEFINED

	ars.reset(rs);
	cout << endl << "----------------------------------------" << endl << endl;
	cout << "Running tests with existing record store:" << endl;
	if (runTests(rs) != 0)
		return (EXIT_FAILURE);

#ifdef ARCHIVEECORDSTORETEST
	/*
	 * Test vacuuming an ArchiveRecordStore
	 */
	cout << "Vacuuming ArchiveRecordStore... " << endl;
	try {
		IO::ArchiveRecordStore::vacuum(rsname, "");
	} catch (Error::ObjectDoesNotExist &e) {
		cout << "Caught: " << e.getInfo() << endl;
	} catch (Error::StrategyError &e) {
		cout << "Caught: " << e.getInfo() << endl;
	}
	cout << "Space usage after vacuum is ";
	try {
		cout << rs->getSpaceUsed() << endl;
	} catch (Error::StrategyError& e) {
		cout << "failed:" << e.getInfo() << "." << endl;
	}
#endif

	/*
	 * Test merging many RecordStores
	 */
	cout << "Test merging many RecordStores... ";
	testMerge();

	/*
	 * Test the deletion of a record store.
	 */
	cout << "Removing store... " << endl;
	try {
		IO::RecordStore::removeRecordStore(rsname, "");
	} catch (Error::ObjectDoesNotExist &e) {
		cout << "Caught: " << e.getInfo() << endl;
	} catch (Error::StrategyError &e) {
		cout << "Caught: " << e.getInfo() << endl;
	}
	cout << "You should see a failure to write the control file... "<< endl;
	return(EXIT_SUCCESS);
#endif
}
