#ifndef _EX_H
#define _EX_H

class Ex
{
public:
	Ex();
	~Ex();
	static const Cinfo* initCinfo();

	void process(const Eref& e, ProcPtr p);
	void reinit(const Eref& e, ProcPtr p);

	double getX() const;
	int getN() const;
	void setX(double x);
	void setN(int n);
	void setNX(int n, double x);
	double getVal(unsigned int index) const;
	void setVal(unsigned int index, double val);
	void getVal(unsigned int index, double val);

	void handleX(double x);
	void handleN(int n);

private:
	double x_;
	int n_;
	vector< double > values_;
};

#endif
