#include "stdafx.h"
#include <fstream>
#include "MlcmShell.h"

using namespace std;

MlcmShell::MlcmShell()
{
	mModel = new Mlcm();
	mFitness = new Fitness(mModel);
	mP.clear();
	mDat.clear();
	mDatFormat = 1;
	mPcpFormat = 1;
	mETFormat = 1;
	mAslopeFormat = 1000000;
	mOutFormat = 1;
	mMeasPerDay = 24;
	mDatBeg[0] = -1;
	mInfileFormat = 1;
	mOutfileFormat = 1;
}

MlcmShell::~MlcmShell()
{
	delete mModel;
	delete mFitness;
}

void MlcmShell::setDefFitnessType(const int &defFitnType)
{
	mFitness->setDefFitnessType(defFitnType);
}

void MlcmShell::setValType(const int &valType)
{
	mFitness->setValType(valType);
}

void MlcmShell::setInOutFormat(const int &inFormat, const int &outFormat)
{
	mInfileFormat = inFormat;
	mOutfileFormat = outFormat;
}

void MlcmShell::setMaxAandZ(const int *newMaxA, const int *newMaxZ)
{
	if (newMaxA[0] != -1)
		Element::maxA[0] = newMaxA[0];
	for (int i = 0; i < 10; i++) {
		if (newMaxA[i + 1] != -1)
			Element::maxA[i + 1] = newMaxA[i + 1];
		if (newMaxZ[i] != -1)
			Element::maxZ[i] = newMaxZ[i];
	}
}

void MlcmShell::setCLim(const double &c1, const double &c2)
{
	if (c1 > c2)
		return;
	if ((c1 >= 0) && (c1 <= 1))
		Element::minC = c1;
	if ((c2 >= 0) && (c2 <= 1))
		Element::maxC = c2;
}

bool MlcmShell::printPrediction(const int *begDate, const int *endDate)
{
	int dayBeg = makeTheGap(begDate, mPcpBeg);
	int dayEnd = makeTheGap(endDate, mPcpBeg);
	if ((dayBeg < 1) || (dayEnd * mMeasPerDay > mP.size()))
		return 0;
	ofstream rout(mOutput, ios::out);
	vector<double> res;
	res = mModel->makeRunoff((--dayBeg) * mMeasPerDay, dayEnd * mMeasPerDay);
	rout.setf(ios::fixed, ios::floatfield);
	rout.precision(3);
	int *nowDate = new int [3];
	nowDate[0] = begDate[0] - 1;
	nowDate[1] = begDate[1];
	nowDate[2] = begDate[2];
	int i;
	for (i = 0; i < (dayEnd - dayBeg) * mMeasPerDay - 1; i++) {
		if (i % mMeasPerDay == 0)
			incDate(nowDate);
		writeOutFormat(rout, nowDate, i, dayBeg * mMeasPerDay, res[i]);
		rout << endl;
	}
	if (i % mMeasPerDay == 0)
			incDate(nowDate);
		writeOutFormat(rout, nowDate, i, dayBeg * mMeasPerDay, res[i]);
		rout << endl;
	rout.close();
	return 1;
}

double MlcmShell::printPrediction(const int *begDate, const int *endDate, const int &fitnessType)
{
	int dayBeg = makeTheGap(begDate, mPcpBeg);
	int dayEnd = makeTheGap(endDate, mPcpBeg);
	if((--dayBeg < 0)
		|| (--dayEnd * mMeasPerDay > mP.size())
		|| (dayBeg - mGap < 0)
		|| ((dayEnd - mGap - dayBeg + 1) * mMeasPerDay > mDat.size()))
		return 0;
	ofstream rout(mOutput, ios::out);
	vector<double> res;
	res = mModel->makeRunoff((--dayBeg) * mMeasPerDay, dayEnd * mMeasPerDay);
	rout.setf(ios::fixed, ios::floatfield);
	rout.precision(3);
	double val;
	if (fitnessType == -1)
		val = mFitness->getValFitness(res);
	else
		val = mFitness->getFitness(fitnessType, res);
	rout << val << endl;
	int *nowDate = new int [3];
	nowDate[0] = begDate[0] - 1;
	nowDate[1] = begDate[1];
	nowDate[2] = begDate[2];
	int i;
	for (i = 0; i < (dayEnd - dayBeg) * mMeasPerDay - 1; i++) {
		if (i % mMeasPerDay == 0)
			incDate(nowDate);
		writeOutFormat(rout, nowDate, i, dayBeg * mMeasPerDay, res[i]);
		rout << endl;
	}
	if (i % mMeasPerDay == 0)
			incDate(nowDate);
		writeOutFormat(rout, nowDate, i, dayBeg * mMeasPerDay, res[i]);
		rout << endl;
	rout.close();
	return val;
}

bool MlcmShell::loadParametrs(const char *paramFile)
{
	ifstream pin(paramFile, ios::in);
	int N;
	double a, c, k, e, t;
	pin >> N >> a >> c >> k >> e;
	if (!pin.ios::eof())
		pin >> t;
	else {
		pin.close();
		return 0;
	}
	vector<double> Al(N), Z(N);
	for (int i = 0; i < N - 1; i++)
		pin >> Al[i] >> Z[i];
	if (N - 1 > 0) {
		pin >> Al[N - 1];
		if (!pin.ios::eof())
			pin >> Z[N - 1];
		else {
			pin.close();
			return 0;
		}
	}
	Element nElement(N, a, c, k, e, t, Al, Z);
	mModel->setParam(nElement);
	pin.close();
	return 1;
}

void MlcmShell::changeModelParametrs(const Element &element)
{
	mModel->setParam(element);
}

double MlcmShell::getFitness() const
{
	return mFitness->getFitness();
}

double MlcmShell::getValFitness() const
{
	return mFitness->getValFitness();
}

bool MlcmShell::setFitnessBegEnd(const int *begDate, const int *endDate)
{
	int begDay = makeTheGap(begDate, mPcpBeg);
	int endDay = makeTheGap(endDate, mPcpBeg);
	if((--begDay < 0)
		|| (--endDay * mMeasPerDay > mP.size())
		|| (begDay - mGap < 0)
		|| ((endDay - mGap - begDay + 1) * mMeasPerDay > mDat.size()))
		return 0;
	mFitness->setBegEnd(begDay, endDay);
	return 1;
}

void MlcmShell::printParams(const char *outputParamFile)
{
	Element params = mModel->getParam();
	ofstream pout (outputParamFile, ios::out);
	int N = params.getN();
	pout << N << endl;
	pout << params.getAlpha0() << endl;
	pout << params.getC() << endl;
	pout << params.getK() << endl;
	pout << params.getEtta() << endl;
	pout << params.getT();
	vector<double> Alpha = params.getAlphas();
	vector<double> Z = params.getZ();
	for (int i = 0; i < N; i++)
		pout << endl << Alpha[i] << " " << Z[i];
	pout.close();
}

Element MlcmShell::getParams() const {
	return mModel->getParam();
}

void MlcmShell::getFitnessTypes(int &defType, int &valType) const
{
	mFitness->getFitnessTypes(defType, valType);
}

void MlcmShell::getInAndOutFormat(int &in, int &out) const
{
	in = mInfileFormat;
	out = mOutfileFormat;
}

void MlcmShell::getMaxAandZ(int *maxA, int *maxZ) const
{
	for (int i = 0; i < 10; i++) {
		maxA[i] = Element::maxA[i];
		maxZ[i] = Element::maxZ[i];
	}
	maxA[10] = Element::maxA[10];
}

void MlcmShell::getCLim(double &c1, double &c2) const
{
	c1 = Element::minC;
	c2 = Element::maxC;
}

void MlcmShell::setOutFile(char *outFileName)
{
	mOutput = outFileName;
}

double MlcmShell::makeET(const int &month)
{
	if ((month < 0) || (month > 11))
		return -1;
	return mET[month - 1];
}

void MlcmShell::readDeck(const double &format, const char *filename)
{
	mETFormat = format;
	ifstream deckIn (filename, ios::in);
	double Aslope, dt, tmp;
	int nuh;
	deckIn >> Aslope >> dt >> nuh;
	mAslope = Aslope * mAslopeFormat;
	mModel->setAslopeAndNuh(mAslope, nuh);
	mMeasPerDay = 24.0 / dt;
	mET.clear();
	for (int i = 0; i < 12; i++) {
		deckIn >> tmp;
		mET.push_back(tmp * mETFormat);
	}
	mFitness->setMeasPerDay(mMeasPerDay);
	deckIn.close();
}

void MlcmShell::readInFormat(ifstream &fin, int &code, int &month, int &day, double &value) const
{
	switch (mInfileFormat) {
	case 1:
		fin >> code >> month >> day >> value;
		return;
	case 2:
		fin >> month >> day >> value;
		code = 0;
		return;
	case 3:
		fin >> value;
		code = 0;
		month = 113;
		day = 1;
		return;
	}
}

void MlcmShell::writeOutFormat(ofstream &fout, const int *date, const int &i, const int &begPoint, const double &value) const
{
	int nMonth = date[1] * 100 + date[2] % 100;
	switch (mOutfileFormat) {
	case 1:
		fout << mCode << " " << nMonth << " " << date[0] << " " << value * mOutFormat;
		return;
	case 2:
		fout << nMonth << " " << date[0] << " " << value * mOutFormat;
		return;
	case 3:
		fout << value;
		return;
	case 4:
		fout << nMonth << " " << date[0] << " " << mP[begPoint + i] * mOutFormat << " "
			<< getRealData(begPoint + mGap * mMeasPerDay + i) * mOutFormat << " " << value * mOutFormat;
		return;
	}
}

double MlcmShell::getRealData(const int &i) const
{
	if ((i < 0) || (i >= mDat.size()))
		return 0;
	return mDat[i];
}

bool MlcmShell::readPcp(const double &format, const char *filename)
{
	mPcpFormat = format;
	ifstream pcpIn (filename, ios::in);
	vector<double> ET;
	int code, month, day;
	double tmp;
	readInFormat(pcpIn, code, month, day, tmp);
	mCode = code;
	mPcpBeg[0] = day;
	mPcpBeg[1] = month / 100;
	mPcpBeg[2] = month % 100;
	if (mPcpBeg[2] > 50)
		mPcpBeg[2] += 1900;
	else
		mPcpBeg[2] += 2000;
	mP.clear();
	mP.push_back(tmp * mPcpFormat);
	ET.push_back(makeET(month / 100));
	if (ET[0] == -1) {
		pcpIn.close();
		mP.clear();
		return 0;
	}
	int i = 0;
	while (!pcpIn.ios::eof()) {
		readInFormat(pcpIn, code, month, day, tmp);
		mP.push_back(tmp * mPcpFormat);
		ET.push_back(makeET(month / 100));
		if (i == 1000000) {
			pcpIn.close();
			mP.clear();
			return 0;
		}
	}
	mModel->setPandET(&mP, &ET);
	if (!mDat.empty()) {
		mGap = makeTheGap(mDatBeg, mPcpBeg);
		mModel->setRealData(&mDat, --mGap * mMeasPerDay);
		mFitness->setRealVal(&mDat, mGap);
	}
	pcpIn.close();
	return 1;
}

bool MlcmShell::readDat(const double &format, const char *filename)
{
	mDatFormat = format;
	mOutFormat = 1.0 / format;
	ifstream datIn (filename, ios::in);
	int code, month, day;
	double tmp;
	readInFormat(datIn, code, month, day, tmp);
	mDat.clear();
	mDatBeg[0] = day;
	mDatBeg[1] = month / 100;
	mDatBeg[2] = month % 100;
	if (mDatBeg[2] > 50)
		mDatBeg[2] += 1900;
	else
		mDatBeg[2] += 2000;
	int i = 0;
	while (!datIn.ios::eof()) {
		readInFormat(datIn, code, month, day, tmp);
		mDat.push_back(tmp * mDatFormat);
		if (i == 1000000) {
			datIn.close();
			mDat.clear();
			mDatBeg[0] = -1;
			return 0;
		}
	}
	datIn.close();
	mGap = makeTheGap(mDatBeg, mPcpBeg);
	mModel->setRealData(&mDat, --mGap * mMeasPerDay);
	mFitness->setRealVal(&mDat, mGap);
	return 1;
}

int MlcmShell::makeTheGap(const int *date1, const int *date2) const		//positive if date1 is after date2
{
	if ((date1[0] == -1) || (date2[0] == -1)) {
		return -1;
	}
	int absDate1, absDate2;
	absDate1 = date1[0] + date1[1] * 50 + date1[2] * 600;
	absDate2 = date2[0] + date2[1] * 50 + date2[2] * 600;
	int gap = 0, i;
	if (absDate1 == absDate2)
		return 1;
	if (absDate2 < absDate1) {
		for (i = date2[2]; i < date1[2]; i++) {
			gap += 365;
			if (i % 4 == 0)
				gap += 1;
		}
		for (i = 1; i < date1[1]; i++)
			gap += giveDaysInMonth(i, date1[2]);
		gap += date1[0];
		for (i = 1; i < date2[1]; i++)
			gap -= giveDaysInMonth(i, date2[2]);
		gap -= date2[0] - 1;
	}
	else {
		for (i = date1[2]; i < date2[2]; i++) {
			gap += 365;
			if (i % 4 == 0)
				gap += 1;
		}
		for (i = 1; i < date2[1]; i++)
			gap += giveDaysInMonth(i, date2[2]);
		gap += date2[0];
		for (i = 1; i < date1[1]; i++)
			gap -= giveDaysInMonth(i, date1[2]);
		gap -= date1[0] + 1;
		gap = -gap;
	}
	return gap;
}


int MlcmShell::giveDaysInMonth(const int &month, const int &year) const
{
	switch (month) {
	case 1: return 31;
	case 2:
		if (year % 4 == 0)
			return 29;
		else
			return 28;
	case 3: return 31;
	case 4: return 30;
	case 5: return 31;
	case 6: return 30;
	case 7: return 31;
	case 8: return 31;
	case 9: return 30;
	case 10: return 31;
	case 11: return 30;
	case 12: return 31;
	default: return 30;
	}
}

void MlcmShell::incDate(int *date) const
{
	if (date[0]++ == giveDaysInMonth(date[1], date[2])) {
		date[0] = 1;
		if (date[1]++ == 12) {
			date[1] = 1;
			date[2]++;
		}
	}
}

int MlcmShell::click()
{
	return mModel->click();
}