#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>

#define IN 2 //num. of input neuron units
#define HN 3 //num. of hidden neuron units
#define ON 1 //num. of output neuron units

using namespace std;

double input[IN]; //input layer
double weight_ih[IN][HN]; //weight from input to hidden
double delta_ih[IN][HN]; //gradient of weight from input to hidden
double output_ho[HN]; //output from hidden to output
double weight_ho[HN][ON]; //weight from hidden to output
double delta_ho[HN][ON]; //gradient of weight from hidden to output
double output[ON]; //output layer
double t[ON];
double loss;

double learning_rate = 1;

double X1[4] = {0,1,0,1};
double X2[4] = {1,0,0,1};
double Y[4] = {1,1,0,0};

int initialize()
{
	srand( (unsigned)time( NULL ) );

	for(int i=0;i<HN;i++)
	{
		for(int j=0;j<IN;j++)
			weight_ih[j][i] = rand()/double(RAND_MAX);
 	}
	for(int i=0;i<ON;i++)
	{
		for(int j=0;j<HN;j++)
			weight_ho[j][i] = rand()/double(RAND_MAX);
	}
	return 0;
}

int forward_propagation()
{
	//TODO: here's the first part you need to implement :)
	return 0;
}

int loss_computation()
{
	loss = 0.0;
	for(int i=0;i<ON;i++)
		loss += 0.5*(output[i]-t[i])*(output[i]-t[i]);
	return 0;
}

int backward_propagation()
{
	//TODO: and here's the second part XD
	return 0;
}

int update()
{
	for(int i=0;i<ON;i++)
	{
		for(int j=0;j<HN;j++)
			weight_ho[j][i] -= learning_rate*delta_ho[j][i];
	}
	for(int i=0;i<HN;i++)
	{
		for(int j=0;j<IN;j++)
			weight_ih[j][i] -= learning_rate*delta_ih[j][i];
	}
	return 0;
}

double sum_loss = 0;

int print()
{
	cout<<"input: "<<input[0]<<" "<<input[1]<<" ground truth: "<<t[0]<<endl;
	cout<<"output: "<<output[0]<<endl;
	cout<<"loss: "<<loss<<endl;
	return 0;
}

int main()
{
	//initialize
	initialize();
	//training
	for(int i=0;i<10000;i++)
	{
		sum_loss = 0;
		for(int j=0;j<4;j++)
		{
			input[0] = X1[j];
			input[1] = X2[j];
			t[0] = Y[j];
			forward_propagation();
			loss_computation();
			backward_propagation();
			update();
			sum_loss += loss;
			if(i%100==0)
			print();
		}
		if(i%100==0)
			cout<<"sum_loss: "<<sum_loss<<endl;
	}	
	return 0;
}
