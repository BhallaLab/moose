#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "../basecode/LookupElementValueFinfo.h"
#include "Ex.h"

Ex::Ex()
	: x_(.0), n_(0)
{
	values_.resize(10, .0);
}

Ex::~Ex() {}

static SrcFinfo1< double >* outputOut()
{
	static SrcFinfo1< double > outputOut(
		"output",
		"Source output"
	);
	return &outputOut;
}

const Cinfo* Ex::initCinfo()
{
	// Value field def
	static ValueFinfo< Ex, int > n(
		"n",
		"Integer element.",
		&Ex::setN,
		&Ex::getN
	);

	static ValueFinfo< Ex, double > x(
		"x",
		"Double element.",
		&Ex::setX,
		&Ex::getX
	);

	// Lookup field def
	static LookupValueFinfo< Ex, unsigned int, double > values(
		"value",
		"Vector of values",
		&Ex::setVal,
		&Ex::getVal
	);

	// Dest field def
	static DestFinfo handleX(
		"handleX",
		"Sets value of x_",
		new OpFunc1< Ex, double >(&Ex::handleX)
	);

	static DestFinfo handleN(
		"handleN",
		"Sets value of n_",
		new OpFunc1< Ex, int >(&Ex::handleN)
	);

	static DestFinfo handleValues(
		"handleValues",
		"Handle a vector of values",
		new OpFunc2< Ex, unsigned int, double >(&Ex::setVal)
	);

	// Shared field def
	static DestFinfo process(
		"process",
		"Handles process call",
		new ProcOpFunc< Ex >(&Ex::process)
	);

	static DestFinfo reinit(
		"reinit",
		"Handles reinit call",
		new ProcOpFunc< Ex >(&Ex::reinit)
	);

	static Finfo* processShared[] = {
		&process,
		&reinit
	};

	static SharedFinfo proc(
		"proc",
		"Handles 'reinit' and 'process' calls from a clock.",
		processShared,
		sizeof(processShared) / sizeof(Finfo*)
	);

	static Finfo* exFinfos[] = {
		&n,
		&x,
		&values,
		&handleX,
		&handleN,
		&handleValues,
		outputOut(),
		&proc
	};

	static Dinfo< Ex > exDinfo;

	static string doc[] =
    {
        "Name", "Ex",
        "Author", "Viktor Toth",
        "Description", "Example Moose class.",
    };

	static Cinfo exCinfo(
		"Ex",
		Neutral::initCinfo(),
		exFinfos,
		sizeof(exFinfos) / sizeof(Finfo*),
		&exDinfo,
		doc,
		sizeof(doc) / sizeof(string)
	);

	return &exCinfo;
}

static const Cinfo* exCinfo = Ex::initCinfo();

void Ex::process(const Eref& e, ProcPtr p)
{
	for (vector<double>::iterator it = values_.begin(); it != values_.end(); ++it)
	{
		(*it) += 1.0;
	}
}

void Ex::reinit(const Eref& e, ProcPtr p)
{
	values_.clear();
}

double Ex::getX() const
{
	return x_;
}

int Ex::getN() const
{
	return n_;
}

void Ex::setX(double x)
{
	x_ = x;
}

void Ex::setN(int n)
{
	n_ = n;
}

void Ex::setNX(int n, double x)
{
	n_ = n;
	x_ = x;
}

double Ex::getVal(unsigned int index) const
{
	if (values_.size() <= index)
	{
		throw "Out of index!";
	}

	return values_[index];
}

void Ex::setVal(unsigned int index, double val)
{
	if (values_.size() <= index)
	{
		throw "Out of index!";
	}

	values_[index] = val;
}
