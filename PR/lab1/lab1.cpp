// lab1.cpp: 实现前向后向、测试不同的学习率，网络层
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <cstdio>

#define IN 2 //num. of input neuron units
#define HN 1 //num. of hidden neuron units
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

double learning_rate;


double X1[4] = { 0,1,0,1 };
double X2[4] = { 1,0,0,1 };
double Y[4] = { 1,1,0,0 };

int initialize()
{
	srand((unsigned)time(NULL));

	for (int i = 0; i<HN; i++)
	{
		for (int j = 0; j<IN; j++)
			weight_ih[j][i] = rand() / double(RAND_MAX);
	}
	for (int i = 0; i<ON; i++)
	{
		for (int j = 0; j<HN; j++)
			weight_ho[j][i] = rand() / double(RAND_MAX);
	}
	return 0;
}

int forward_propagation()
{	
	for (int i = 0; i < HN; i++) {
		double temp_sum = 0.0;
		for (int j = 0; j < IN; j++) {
			temp_sum += input[j] * weight_ih[j][i];
		}

		output_ho[i] = 1/(1 + exp( -1 * temp_sum));
	}
	for (int i = 0; i < ON; i++) {
		double temp_sum = 0.0;
		for (int j = 0; j < HN; j++) {
			temp_sum += output_ho[j] * weight_ho[j][i];
		}

		output[i] = 1 / (1 + exp(-1 * temp_sum));
	}
	return 0;
}

int loss_computation()
{
	loss = 0.0;
	for (int i = 0; i<ON; i++)
		loss += 0.5*(output[i] - t[i])*(output[i] - t[i]);
	return 0;
}

int backward_propagation()
{
	//-----layer2
	double sigema2[ON];
	for (int j = 0; j < ON; j++) {
		sigema2[j] = (output[j] - t[j])*output[j] * (1 - output[j]);
	}
	for (int i = 0; i < HN; i++) {
		for (int j = 0; j < ON; j++) {
			delta_ho[i][j] = sigema2[j] * output_ho[i];
		}
	}
	//-----layer1
	double sigema1[HN];
	for (int j = 0; j < HN; j++) {
		sigema1[j] = 0.0;
		for (int i = 0; i < ON; i++) {
			sigema1[j] += weight_ho[j][i] * sigema2[i] * output_ho[j] * (1 - output_ho[j]);
		} 
	}
	for (int i = 0; i < IN; i++) {
		for (int j = 0; j < HN; j++) {
			delta_ih[i][j] = sigema1[j] * input[i];
		}
	}

	return 0;
}

int update()
{
	for (int i = 0; i<ON; i++)
	{
		for (int j = 0; j<HN; j++)
			weight_ho[j][i] -= learning_rate * delta_ho[j][i];
	}
	for (int i = 0; i<HN; i++)
	{
		for (int j = 0; j<IN; j++)
			weight_ih[j][i] -= learning_rate * delta_ih[j][i];
	}
	return 0;
}

double sum_loss = 0;

int print()
{
	cout << "input: " << input[0] << " " << input[1] << " ground truth: " << t[0] << endl;
	cout << "output: " << output[0] << endl;
	cout << "loss: " << loss << endl;
	return 0;
}
void nn() {
	for (int i = 0; i<10000; i++)
	{
		sum_loss = 0;
		for (int j = 0; j<4; j++)
		{
			input[0] = X1[j];
			input[1] = X2[j];
			t[0] = Y[j];
			forward_propagation();
			loss_computation();
			backward_propagation();
			update();
			sum_loss += loss;
			if (i % 9999 == 0)
				print();
		}
		if (i % 1000 == 0)
			//printf("%.10f,",sum_loss);
;			//cout << sum_loss <<",";
	}
}

int main()
{
	//freopen("o.csv", "w", stdout);
	//initialize
	
	//training
	double lrs[2] = {15,20};
	//int HNs[5] = { 1,2,3,5,10 };


		for (int lr = 0; lr < 2; lr++) {
			initialize();
			learning_rate = lrs[lr];
			cout << learning_rate << ",";
			nn();
			cout << endl;
			print();
		}

	return 0;
}
