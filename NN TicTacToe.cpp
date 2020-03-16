/*
	by Maciej Rozpedek
	02.2020 - 03.2020
*/
#include <iostream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <conio.h>
#include <fstream>
#include <io.h>
#include <string>

using namespace std;

class neuron {
	public:
		double error;
		double output;
		vector<double> inputs;
		vector<double> weights;
		void alocateFirstsInputs(vector<double> firstInputs){	//	pobiera wejscia -- warstwa 1
			inputs.clear();
			for(int i=0;i<firstInputs.size();i++){
				inputs.push_back(firstInputs[i]);
			}

		}
		void alocateInputs(int numberOfNeurons, neuron neuronTable[]){	//	pobiera wejscia -- warstwy 2+
			inputs.clear();
			for(int i=0;i<numberOfNeurons;i++){
				inputs.push_back(neuronTable[i].output);
			}
		}
		void randomWeight(int numberOfNeurons){
			for(int i=0;i<numberOfNeurons;i++){
				weights.clear();
				weights.reserve(numberOfNeurons);
				weights[i]=rand()%2001;					//	losowa wago od -1.0 do 1.0
				weights[i]/=1000;
				weights[i]-=1;							// 	z zaokragleniem do 0.001
			}
		};

		double sigmoidFunction(double output){
			double exp_value;
			double return_value;
			exp_value = exp((double) -output);				//	funkcja sigmoidalna
			return_value = 1 / (1 + exp_value);
			return return_value;
		}

		void calcOutput(){
			output=0;
			for(int i=0;i<inputs.size();i++){
				output+=inputs[i]*weights[i];		//	liczenie wyjscia
			}
			output = sigmoidFunction(output);
		};
		void mutateNeuron(int error){
			double keeper;
			for(int i=0;i<weights.capacity();i++){		//  funkcja mutuj¹ca
				keeper = rand()%(error*2+1);
				keeper -= error;
				keeper /= 1000;
				weights[i] += keeper;
				if(weights[i]>1){
					weights[i]=1;
				}
				if(weights[i]<-1){
					weights[i]=-1;
				}
			}
		}
};

void mutateNetwork(neuron **neuronTable, int error, int numberOfLayers, int numberOfNeurons, int numberOfInputs, int numberOfOutputs){
	for(int i=1;i<numberOfLayers-1;i++){				// funkcja do mutowania calej sieci
		for(int j=0;j<numberOfNeurons;j++){
			neuronTable[i][j].mutateNeuron(error);	//warstwy 1 -> n-1
		}
	}
	for(int j=0;j<numberOfOutputs;j++){
		neuronTable[numberOfLayers-1][j].mutateNeuron(error);	// warstwa n
	}
}

void neuralNetActivation(vector<double>firstInputs, neuron **neuronTable, int numberOfLayers, int numberOfNeurons, int numberOfOutputs){
	for(int j=0;j<numberOfNeurons;j++){
		neuronTable[0][j].alocateFirstsInputs(firstInputs);		//  alokacja wejscia (warstwya 1)
		neuronTable[0][j].calcOutput();							//  wyliczenie wyjscia (warstwa 1)
	}
	for(int i=1;i<numberOfLayers-1;i++){										//*************wyliczenie wyjœcia z sieci*************//
		for(int j=0;j<numberOfNeurons;j++){
			neuronTable[i][j].alocateInputs(numberOfNeurons, neuronTable[i-1]);	//  alokacja wejscia (warstwy 2+)
			neuronTable[i][j].calcOutput();										// wylicznie wyjscia (warstwy 2+)
		}
	}
	for(int j=0;j<numberOfOutputs;j++){
		neuronTable[numberOfLayers-1][j].alocateInputs(numberOfNeurons, neuronTable[numberOfLayers-2]);
		neuronTable[numberOfLayers-1][j].calcOutput();
	}
}

void writeTheBoard(vector <double> outputs){
	cout << "-------------------";							// wypisanie planszy do kolko i krzyrzyk
	for(int i=0;i<9;i++){									// 0-9 = X
		cout << "\n|     |     |     |\n";					// 10-18 = O
		for(int j=0;j<3;j++){
			cout << "|  ";
			if(outputs[i]==1){
				cout << "X";
			}
			if(outputs[i+9]==1){
				cout << "O";
			}
			if(outputs[i]!=1&&outputs[i+9]!=1){
				cout << " ";
			}
			if(j!=2){
				i++;
			}
			cout << "  ";
		}
		cout << "|\n|     |     |     |\n-------------------";
	}
	cout << "\n";
}

void backPropagation(neuron **neuronTable, vector <double> exitErrors, int numberOfLayers, int numberOfNeurons, int numberOfInputs, int numberOfOutputs){
	for(int j=0;j<numberOfOutputs;j++){
		neuronTable[numberOfLayers-1][j].error=exitErrors[j];
	}
	for(int j=0;j<numberOfNeurons;j++){
		neuronTable[numberOfLayers-2][j].error=0;
		for(int k=0;k<numberOfOutputs;k++){
			neuronTable[numberOfLayers-2][j].error+=neuronTable[numberOfLayers-1][k].weights[j]*neuronTable[numberOfLayers-1][k].error;
		}
	}
	for(int i=numberOfLayers-2;i>0;i--){
		for(int j=0;j<numberOfNeurons;j++){
			neuronTable[i][j].error=0;
			for(int k=0;k<numberOfNeurons;k++){
				neuronTable[i][j].error+=neuronTable[i+1][k].weights[j]*neuronTable[i+1][k].error;
			}
		}
	}
	for(int i=0;i<numberOfLayers-1;i++){
		for(int j=0;j<numberOfNeurons;j++){
			neuronTable[i][j].mutateNeuron(neuronTable[i][j].error);
		}
	}
	for(int j=0;j<numberOfOutputs;j++){
		neuronTable[0][j].mutateNeuron(neuronTable[0][j].error);
	}
}

int makeMove(vector <double> ticTacToeBoard, vector <double> outputs, bool noughtOrCrosse){
	double crossMoveValue = -1;
	double noughtMoveValue = -1;
	int crossMove;
	int noughtMove;
	for(int i=0;i<ticTacToeBoard.size()/2;i++){
		if(ticTacToeBoard[i]==0&&ticTacToeBoard[i+9]==0){
			if(outputs[i]>crossMoveValue){
				crossMoveValue=outputs[i];
				crossMove=i;
			}
		}
	}
	for(int i=ticTacToeBoard.size()/2;i<ticTacToeBoard.size();i++){
		if(ticTacToeBoard[i]==0&&ticTacToeBoard[i-9]==0){
			if(outputs[i]>noughtMoveValue){
				noughtMoveValue=outputs[i];
				noughtMove=i;
			}
		}
	}
	if(noughtOrCrosse==0){
		ticTacToeBoard[noughtMove]=1;
		return noughtMove;
	}
	else{
		ticTacToeBoard[crossMove]=1;
		return crossMove;
	}
}

int whoWin (vector <double> ticTacToeBoard){
	for(int a=0;a<9;a+=3){
		if(ticTacToeBoard[a]==1&&ticTacToeBoard[a+1]==1&&ticTacToeBoard[a+2]==1){
			return 1;							// krzyzyk	0-8
		}
		if(ticTacToeBoard[a+9]==1&&ticTacToeBoard[a+10]==1&&ticTacToeBoard[a+11]==1){
			return 2;							// kolko	9-17
		}
	}
	for(int a=0;a<3;a++){
		if(ticTacToeBoard[a]==1&&ticTacToeBoard[a+3]==1&&ticTacToeBoard[a+6]==1){
			return 1;
		}
		if(ticTacToeBoard[a+9]==1&&ticTacToeBoard[a+12]==1&&ticTacToeBoard[a+15]==1){
			return 2;
		}
	}
	if(ticTacToeBoard[0]==1&&ticTacToeBoard[4]==1&&ticTacToeBoard[8]==1){
		return 1;
	}
	if(ticTacToeBoard[2]==1&&ticTacToeBoard[4]==1&&ticTacToeBoard[6]==1){
		return 1;
	}
	if(ticTacToeBoard[9]==1&&ticTacToeBoard[13]==1&&ticTacToeBoard[17]==1){
		return 2;
	}
	if(ticTacToeBoard[11]==1&&ticTacToeBoard[13]==1&&ticTacToeBoard[15]==1){
		return 2;
	}
	return 0;
}

bool fight(bool withPlayer, char Xor0, bool showBoard, int error, neuron **neuronTableOne, neuron **neuronTableTwo, bool lastWinner, int numberOfLayers, int numberOfNeurons, int numberOfInputs, int numberOfOutputs){
	vector<double> firstInputs;
	vector<double> endOutputs;
	int line;
	int column;
	firstInputs.reserve(numberOfInputs);
	endOutputs.reserve(numberOfOutputs);
	for(int i=0;i<numberOfInputs;i++){
		firstInputs.push_back(0);
	}
	for(int i=0;i<9;i++){
		if(withPlayer==0||Xor0=='0'){
			neuralNetActivation(firstInputs, neuronTableOne, numberOfLayers, numberOfNeurons, numberOfOutputs);
			endOutputs.clear();
			for(int j=0;j<numberOfOutputs;j++){
				endOutputs.push_back(neuronTableOne[numberOfLayers-1][j].output);	// zczytanie wyjsc po aktywacji sieci
			}
		}
		else{
			if(Xor0=='X'){
				cout << "Enter line and column\n";
				while(!(cin >> line)){
					cin.clear();
					cin.ignore();
				}
				while(!(cin >> column)){
					cin.clear();
					cin.ignore();
				}
				endOutputs.clear();
				for(int j=0;j<9;j++){
					endOutputs.push_back(0);
				}
				line--;
				column--;
				endOutputs[line*3+column]=1;
			}
		}
		firstInputs[makeMove(firstInputs, endOutputs, 1)]=1;
		if(showBoard==1){
            writeTheBoard(firstInputs);
		}
		switch(whoWin(firstInputs)){
			case 0:
				if(lastWinner==0){
					mutateNetwork(neuronTableTwo, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				}
				else{
					mutateNetwork(neuronTableOne, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				}
				break;
			case 1:
				mutateNetwork(neuronTableTwo, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				lastWinner = 0;
				if(withPlayer==1){
					cout << "You win\n";
				}
				return 0;
				break;
			case 2:
				mutateNetwork(neuronTableOne, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				lastWinner = 1;
				if(withPlayer==1){
					cout << "Network win\n";
				}
				return 1;
				break;
		}
		if(i==8){
			continue;
		}
		else{
			i++;
		}
	///////////////////////////////////////////////
	if(withPlayer==0||Xor0=='X'){
			neuralNetActivation(firstInputs, neuronTableTwo, numberOfLayers, numberOfNeurons, numberOfOutputs);
			endOutputs.clear();
			for(int j=0;j<numberOfOutputs;j++){
				endOutputs.push_back(neuronTableTwo[numberOfLayers-1][j].output);	// zczytanie wyjsc po aktywacji sieci
			}
		}
		else{
			if(Xor0=='0'){
				cout << "Enter line and column\n";
				while(!(cin >> line)){
					cin.clear();
					cin.ignore();
				}
				while(!(cin >> column)){
					cin.clear();
					cin.ignore();
				}
				endOutputs.clear();
				for(int j=0;j<numberOfOutputs;j++){
					endOutputs.push_back(0);
				}
				line--;
				column--;
				endOutputs[line*3+column+9]=1;
			}
		}
		firstInputs[makeMove(firstInputs, endOutputs, 0)]=1;
		if(showBoard==1){
            writeTheBoard(firstInputs);
		}
		switch(whoWin(firstInputs)){
			case 0:
				if(lastWinner==0){
					mutateNetwork(neuronTableTwo, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				}
				else{
					mutateNetwork(neuronTableOne, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				}
				break;
			case 1:
				mutateNetwork(neuronTableTwo, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				lastWinner = 0;
				if(withPlayer==1){
					cout << "Network win\n";
				}
				return 0;
				break;
			case 2:
				mutateNetwork(neuronTableOne, error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				lastWinner = 1;
				if(withPlayer==1){
					cout << "You win\n";
				}
				return 1;
				break;
		}
	}
}

void train(int numberOfRepetitions, neuron ***neuronTable, int numberOfLayers, int numberOfNeurons, int numberOfInputs, int numberOfOutputs){
									//*****funkcja trenujaca siec*****//
}

int main(){
	int numberOfNeurons=50;		//*****Liczba neuronow w jednej warstwie*****////	musi byc <10 || >19		//
	int numberOfLayers=3;			//*****Liczba warstw neutronów*****//
	int numberOfInputs=18;
	int numberOfOutputs=18;
	int numberOfNetworks=2;
	bool lastWinner = 0;
	int error;
	int keeper;
	int whatToDo;
	char Xor0;
	string nameOfFile;
	ofstream newFile;
	fstream file;
	int networkNumber;
	neuron ***neuronTable = new neuron ** [numberOfNetworks];
	for(int i=0;i<numberOfNetworks;i++){						//alokacja neuronow		neuronTable[sieci][warstwy][neurony]
		neuronTable[i] = new neuron *[numberOfLayers];
		for(int j=0;j<numberOfLayers-1;j++){
			neuronTable[i][j] = new neuron [numberOfNeurons];
		}
			neuronTable[i][numberOfLayers-1] = new neuron [numberOfOutputs];
			// alokacja wyjsc w ostatniej warstwie  //
	}

	for(int i=0;i<numberOfNetworks;i++){
		for(int j=0;j<numberOfNeurons;j++){
			neuronTable[i][0][j].randomWeight(numberOfInputs);				//losowanie wag dla 1 warstwy
		}
		for(int j=1;j<numberOfLayers-1;j++){
			for(int k=0;k<numberOfNeurons;k++){
				neuronTable[i][j][k].randomWeight(numberOfNeurons);			//losowanie wag dla warstwy 2+
			}
		}
		for(int k=0;k<numberOfOutputs;k++){
			neuronTable[i][numberOfLayers-1][k].randomWeight(numberOfNeurons);
		}
	}
	int h=0;
	error = 100;
	while(true){
		h++;
		if(h<10000000){
			if(h%100==0){
				cout << h << "	";
				if(h%1000==0){
					cout << "\n";
				}
//				cout<<neuronTable[0][2][15].weights[3]<<" ";
			}
		}
		else{
			if(h%200==0){
				cout << h << "	";
				if(h%1000==0){
					cout << "\n";
				}
			}
		}
		if(kbhit()){
			cin.clear();
			cin.ignore();
			whatToDo = 0;
			while(whatToDo!=1){
				cout << "What will you do?\n";
				cout << "0 - see game\n";
				cout << "1 - train neural network\n";
				cout << "2 - mutate NeuralNetwork\n";
				cout << "3 - play with neural network\n";
				cout << "4 - save neural network\n";
				cout << "else - end program\n";
				while(!(cin >> whatToDo)){
						cin.clear();
						cin.ignore();
					}
//				if(whatToDo==1){
//					cout << "Enter the speed of modyfication\n 0 means last value\n";
//					keeper = error;
//					cin >> error;
//					if(error = 0){
//						error = keeper;
//					}
//				}
				if(whatToDo==0){
					fight(0, 0, 1, error, neuronTable[0], neuronTable[1], lastWinner, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
				}
				if(whatToDo==2){
					cout << "choose network (0 or X) and enter error\n";
					while(!(cin >> Xor0)){
						cin.clear();
						cin.ignore();
					}
					while(!(cin >> error)){
						cin.clear();
						cin.ignore();
					}
					if(Xor0=='0'){
						mutateNetwork(neuronTable[1], error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
					}
					else{
						mutateNetwork(neuronTable[0], error, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
					}
				}
				if(whatToDo==3){
					cout << "select X or 0\n";
					while(!(cin >> Xor0)){
						cin.clear();
						cin.ignore();
					}
					if(Xor0=='0'){
						fight(1, '0', 1, error, neuronTable[0], neuronTable[1], lastWinner, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
					}
					else{
						fight(1, 'X', 1, error, neuronTable[0], neuronTable[1], lastWinner, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
					}
				}
				if(whatToDo==4){
					cout << "Enter network number:\n";
					while(!(cin >> networkNumber)){
						cin.clear();
						cin.ignore();
					}
					cout << "Enter name of file\n";
					while(!(cin >> nameOfFile)){
						cin.clear();
						cin.ignore();
					}
					nameOfFile+=".txt";
					newFile.open(nameOfFile.c_str());
					newFile.close();
					file.open(nameOfFile.c_str());
					for(int j=0;j<numberOfNeurons;j++){
						for(int l = 0;l<numberOfInputs;l++){
							file<<neuronTable[networkNumber][0][j].weights[l]<<" ";
						}
					}
					for(int j=1;j<numberOfLayers-1;j++){
						for(int k=0;k<numberOfNeurons;k++){
							for(int l=0;l<numberOfNeurons;l++){
								file<<neuronTable[networkNumber][j][k].weights[l]<<" ";
							}
						}
					}
					for(int k=0;k<numberOfOutputs;k++){
						for(int l=0;l<numberOfNeurons;l++){
							file<<neuronTable[networkNumber][numberOfLayers-1][k].weights[l]<<" ";
						}
					}
					file.close();
				}
			}
		}
		fight(0, 0, 0, error, neuronTable[0], neuronTable[1], lastWinner, numberOfLayers, numberOfNeurons, numberOfInputs, numberOfOutputs);
	}

	return 0;
}
