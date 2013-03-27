#include "stdafx.h"
#include "Hydrograph.h"

Hydrograph::Hydrograph()
{
	mMlcmSh = new MlcmShell();
	mCal = new Calibration(mMlcmSh);
}

Hydrograph::~Hydrograph()
{
	delete mCal;
}

void Hydrograph::setCalibrationType(const int &calType)
{
	mCal->setCalibrationType(calType);
}

void Hydrograph::setFitnessType(const int &fitnDefType, const int &valType)
{
	mMlcmSh->setDefFitnessType(fitnDefType);
	mMlcmSh->setValType(valType);
}

void Hydrograph::setInOutFormat(const int &inFormat, const int &outFormat)
{
	mMlcmSh->setInOutFormat(inFormat, outFormat);
}

void Hydrograph::setMaxAandZ(const int *maxA, const int *maxZ)
{
	mMlcmSh->setMaxAandZ(maxA, maxZ);
}

void Hydrograph::setNMStopAndLim(const double &NMStop, const int &NMLim)
{
	mCal->setNMStopAndLim(NMStop, NMLim);
}

void Hydrograph::setNMKoeffs(const double &koeff1, const double &koeff2)
{
	mCal->setNMKoeffs(koeff1, koeff2);
}

void Hydrograph::setNMKoeffs(const std::vector<double> koeffs)
{
	mCal->setNMKoeffs(koeffs);
}

void Hydrograph::setMinGrowth(const double minGrowth)
{
	mCal->setMinGrowth(minGrowth);
}

void Hydrograph::setCLim(const double &c1, const double &c2)
{
	mMlcmSh->setCLim(c1, c2);
}

void Hydrograph::getCLim(double &c1, double &c2) const
{
	mMlcmSh->getCLim(c1, c2);
}

void Hydrograph::setBruteforceParams(const int &steps, const int &it)
{

}

bool Hydrograph::loadMlcm(const char *paramFile)
{
	return mMlcmSh->loadParametrs(paramFile);
}

void Hydrograph::printMlcm(const char *outputParamFile)
{
	mMlcmSh->printParams(outputParamFile);
}

bool Hydrograph::printPrediction(const int *begDate, const int *endDate)
{
	return mMlcmSh->printPrediction(begDate, endDate);
}

double Hydrograph::printPredAndValid(const int *begDate, const int *endDate, const int &fitnessType)
{
	return mMlcmSh->printPrediction(begDate, endDate, fitnessType);
}

double Hydrograph::calibrate(const int *begDate, const int *endDate)
{
	if(!mMlcmSh->setFitnessBegEnd(begDate, endDate))
		return -1;
	return mCal->doCalibration();
}

double Hydrograph::validate(const int *begDate, const int *endDate)
{
	if(!mMlcmSh->setFitnessBegEnd(begDate, endDate))
		return -1;
	return mMlcmSh->getValFitness();
}

void Hydrograph::getCalAndFitnessTypes(int &calType, int &defFitnType, int &valType) const
{
	calType = mCal->getCalType();
	mMlcmSh->getFitnessTypes(defFitnType, valType);
}

void Hydrograph::getInAndOutFormat(int &in, int &out) const
{
	mMlcmSh->getInAndOutFormat(in, out);
}

void Hydrograph::getMaxAandZ(int *maxA, int *maxZ) const
{
	mMlcmSh->getMaxAandZ(maxA, maxZ);
}

void Hydrograph::getBruteforceParams(int &steps, int &iter) const
{
	steps = 1;
	iter = 1;
}

void Hydrograph::getNMParams(double &stop, int &iter) const
{
	mCal->getNMParams(stop, iter);
}

int Hydrograph::getNMKoeffs(std::vector<double> &koeffs) const
{
	return mCal->getNMKoeffs(koeffs);
}

void Hydrograph::getSlsParams(double &slsStep, int &slsLim, int &slsCalType) const
{
	slsStep = 1;
	slsLim = 1;
	slsCalType = 1;
}

void Hydrograph::setSlsParam(const double &slsStep, const int &slsLim, const int &slsCalType)
{

}

int Hydrograph::getMinGrowth() const
{
	return mCal->getMinGrowth();
}

void Hydrograph::setOutFile(char *outFileName)
{
	mMlcmSh->setOutFile(outFileName);
}

void Hydrograph::readDeck(const double &format, const char *filename)
{
	mMlcmSh->readDeck(format, filename);
}

bool Hydrograph::readPcp(const double &format, const char *filename)
{
	return mMlcmSh->readPcp(format, filename);
}

bool Hydrograph::readDat(const double &format, const char *filename)
{
	return mMlcmSh->readDat(format, filename);
}

int Hydrograph::click()
{
	return mMlcmSh->click();
}