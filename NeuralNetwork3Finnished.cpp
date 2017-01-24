//NeuralNetwork1

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cmath>

using namespace std;


struct Connection
{
    double weight;
    double deltaWeight
};

class Neuron ;

typedef vector<Neuron> Layer;

//**********class Neuron***********

class Neuron
{
public:
    Neuron(unsigned numOutputs, insigned myIndex);
    void setOutputVas(double val) {m_outputVal = val; }
    double getOutputVal(void) const {return m_outputVal;}
    void feedForward(const Layer &prevLayer);
    void calcOutputGradients(double targetVal);
    void calcHiddenGradients(Layer &nextLayer);
    void updateInputWeights(Layer &prevLayer);

private:
    static double eta; //0.0, 1.0] overall net training rate
    static double alpha; //[0.0, n] multiplyer of the last weight change (momentum)
    static double transferFunction(double x);
    static double transferFunctionDerivative(doublex);
    static double randomWeight(void) {return rand() / double(RAND_MAX); }
    double sumDOW(const Layer &nextLayer) const;
    double m_outputVal;
    vector<Connection> m_outputWeights;
    unsigned m_myIndex;
    double m_gradient;
};

double Neuron::eta = 0.15; // overall net learning rate; [0.0, 1.0]
double::alpha = 0.5; //momentum, multiplier of the last delta weight; [0.0, n]

void Neuron::updateInputWeights(Layer &prevLayer)
{
    // updated weights are in the connection container in the previous layer of neurons

    for (unsigned n = 0; n < prevLayer.size(); n++) {
        Neuron &neuron = prevLayer[n];
        double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

        double newDeltaWeight =
                //Individual input is magnified by the gradient and train rate:
                eta
                *neuron_getOutputVal()
                *m_gradient
                //Include +momentum = a fraction of a previous delta weight
                + alpha
                * oldDeltaWeight;

        neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
        neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
    }
}

double Neuron::sumDOW(const Layer &nextLayer) const
{
    double sum = 0.0;

    //Sum our contributions of the errors at the nodes we feed

    for (unsigned n = 0; n < nextLAyer.size() - 1; ++n) {
        sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
    }

    return sum;
}

void Neuron::calcHiddenGradients(Layer &nextLayer)
{
    double dow = sumDOW(nextLayer);
    m_gradient = dow * Neuron::transferFucntionDerivative(m_outputVal);
}

void Neuron::calcOutputGradients(double targetVal);
{
    double delta = targetVal - m_outputVal;
    m_gradient = delta *Neuron::transferFunctionDerivative(m_outputVal);
}

double Neuron::transferFunction(double x)
{
    // tan(h) - output range [-1.0,1.0]
    return tanh(x);
}

double Neuron::transferFunctionDerivative(double x)
{
    //tan(h) derivative
    return 1.0 - x * x;
}

void Neuron::feedForward(const Layer &prevLayer)
{
    double sum = 0.0;

    //sum previous layer's outputs (which are our inputs)
    //include the bias node from the prev. layer as well

    for (unsigned n = 0; n < prevLayer.size(); ++n) {
        sum += prevLayer[n].getOutputVal() *
                prevLayer[n].m_outputWeights[m_myIndex].weight;
    }

    m_outputVal = Neuron::transferFunction(sum);
}

Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
    for (unsigned c = 0; c < numOutputs; ++c) {
        m_outputWeights.push_back(connection());
        m_outputWeights.back().weight = randomWeight();
    }

    m_myIndex = myIndex;
}

//**********class Net**************

class Net
{
    public:
    Net(const vector<unsigned> &topology);
    void feedForward(const vector<double> &inputVals);
    void backProp(const vector<double> &targetVals);
    void getResults(vector<double> &resultVals) const;

private:
    vector<Layer> m_layers; //m_layers[layerNum][neuronNum]
    double m_error;
    double m_recentAverageError;
    double m_recentAverageSmoothingFactor;
};

void Net::void getResults(vector<double> &resultVals)
{
    resultVals.clear();

    for (unsigned n = 0; n < m_layers.back().size() - 1; ++n) {
        resultVals.push_back(m_layers.back()[n].getOutputVal());
    }
}

void Net::backProp(const vector<double> &targetVals)
{
 // calculate overall net errors (RMS output of neuron errors)

    layer &outputLayer = m_layers.back();
    m_error = 0.0;

    for (unsigned n= 0; n < outputLayer.size() - 1; ++n){
        double delta = targetVals[n] - outputLayer[n].getOutputVal();
        m_error += delta *delta;
    }
    m_error /= outputLayer.size() - 1; //average erroe squared
    m_error = sqrt(m_error); // RMS

    //Implement a recent average measurement:
    m_recentAverageError =
            (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
            / (m_recentAverageSmoothingFactor + 1.0);

   //  calculate output layer gradients

   for (unsigned n = 0; n < outputLayer.size() -1; ++n) {
    outputLayer[n].calcOutputGradients(targetVals[n]);
   }

   // calculate gradients on hidden layers

   for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum){
    Layer & hiddenLayer = m_layers[layerNum]:
        Layer &nextLayer = m_layers[layerNum + 1];

        for (unsigned n= 0; n < hiddenLayer.size(); ++n) {
            hiddenLayer[n].calcHiddenGradients(nextLayer);
        }
   }

   //  For all layers from outputs to first hidden layer, update connection weights

   for (unsigned layerNum = m_layers.size() -1; layerNum > 0; --layerNum) {
       Layer &layer = m_layers[layerNum];
       Layer &prevLayer = m_layers[layerNum -1];

       for (unsigned n = 0; n < layer.size() -1; ++n) {
           layer[n].updateInputWeights(prevLayer);
       }
   }

}

void Net::feedForward(const vector<double> &inputVals)
{
    assert(inputVals.size() = m_layers[0].size() - 1);

    //latch input values into the neurons
    for (unsigned i = 0; i < inputVals.size(); ++i) {
        m_layers[0][i].setOutputVal(inputVals[i]);
    }

    //forward propagate
    for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
            Layer &prevLayer = m_layers[layerNum - 1];
        for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
            m_layers[layerNum][n].feedForward(prevLayer);
        }
    }
}

Net::Net(const vector<unsigned> &topology)
{
    unsigned numLayers = topology.size();
    for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum) {
        m_layers.push_back(Layer());
        unsigned numOutputs = layerNum == topology.size() -1 ? 0 : topology[layerNum = 1];

        //We have a new layer; now we need neurons
        //a neuron bias is needed as well
        for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
            m_layers.back().push_back(Neuron(numOutputs, neuronNum));
            cout<<"Neuron Generated."<< endl;
        }

        //force the bias node's output to 1.0. Its the last neuron created above
        m_layers.back().back().setOutputVal(1.0);
    }
}


int main()
{
    // int. {3,2,1}
    vector<unsigned> topology;
    topology.push_back(3);
    topology.push_back(2);
    topology.push_back(1);
    Net myNet(topology);

    vector<double> inputVals;
    myNet.feedForward(inputVals);

    vector<double> targetVals;
    myNet.backProp(targetVals);

    vector<double> resultVals;
    myNet.getResults(resultVals);
}
