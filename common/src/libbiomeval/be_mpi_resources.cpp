/**
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties.  Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain.  NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */
#include <mpi.h>
#include <sstream>

#include <be_mpi.h>
#include <be_io_propertiesfile.h>
#include <be_mpi_resources.h>

namespace BE = BiometricEvaluation;

/*
 * The common properties for MPI Resources.
 */
const std::string
BiometricEvaluation::MPI::Resources::WORKERSPERNODEPROPERTY("Workers Per Node");

/******************************************************************************/
/* Class method definitions.                                                  */
/******************************************************************************/
BiometricEvaluation::MPI::Resources::Resources(
    const std::string &propertiesFileName)
{
	this->_propertiesFileName = propertiesFileName;
	this->_uniqueID = MPI::generateUniqueID();
	this->_rank = ::MPI::COMM_WORLD.Get_rank();
	this->_numTasks = ::MPI::COMM_WORLD.Get_size();

	/* Read the properties file */
	std::auto_ptr<IO::PropertiesFile> props;
	try {
		props.reset(new IO::PropertiesFile(propertiesFileName,
		    IO::READONLY));
	} catch (Error::Exception &e) {
		throw Error::FileError("Could not open properties: " +
		    e.whatString());
	}
	try {
		this->_workersPerNode = props->getPropertyAsInteger(
		    MPI::Resources::WORKERSPERNODEPROPERTY);
	} catch (Error::Exception &e) {
		throw Error::ObjectDoesNotExist("Could not read properties: " +
		    e.whatString());
	}
}

/******************************************************************************/
/* Object method definitions.                                                 */
/******************************************************************************/
BiometricEvaluation::MPI::Resources::~Resources()
{
}

std::string
BiometricEvaluation::MPI::Resources::getPropertiesFileName()
{
	return (_propertiesFileName);
}

std::vector<std::string>
BiometricEvaluation::MPI::Resources::getRequiredProperties()
{
	std::vector<std::string> props;
	props.push_back(MPI::Resources::WORKERSPERNODEPROPERTY);
	return (props);
}

std::string
BiometricEvaluation::MPI::Resources::getUniqueID() const
{
	return (this->_uniqueID);
}

int
BiometricEvaluation::MPI::Resources::getRank() const
{
	return (this->_rank);
}

int
BiometricEvaluation::MPI::Resources::getNumTasks() const
{
	return (this->_numTasks);
}

int
BiometricEvaluation::MPI::Resources::getWorkersPerNode() const
{
	return (this->_workersPerNode);
}
std::shared_ptr<BiometricEvaluation::IO::LogSheet>
BiometricEvaluation::MPI::Resources::getLogSheet() const
{
	return (this->_logSheet);
}


