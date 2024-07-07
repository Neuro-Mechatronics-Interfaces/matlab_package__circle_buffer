#include "mex.h"
#include "CircularBuffer.h"
#include <map>
#include <string>

std::map<std::string, CircularBuffer*> bufferMap;

void createBuffer(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 3) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:createBuffer", "Three input arguments required: buffer name, number of channels, and buffer size.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:createBuffer", "First input argument must be a string (buffer name).");
    }
    if (!mxIsScalar(prhs[1]) || !mxIsScalar(prhs[2])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:createBuffer", "Second and third input arguments must be scalars (number of channels and buffer size).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    size_t numChannels = static_cast<size_t>(mxGetScalar(prhs[1]));
    size_t bufferSize = static_cast<size_t>(mxGetScalar(prhs[2]));
    
    bufferMap[bufferName] = new CircularBuffer(numChannels, bufferSize);
}

void addData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 2) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:addData", "Two input arguments required: buffer name and data array.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:addData", "First input argument must be a string (buffer name).");
    }
    if (!mxIsSingle(prhs[1])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:addData", "Second input argument must be an array of singles (data).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    
    if (bufferMap.find(bufferName) == bufferMap.end()) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:addData", "Buffer not found: %s", bufferName);
    }
    
    CircularBuffer* buffer = bufferMap[bufferName];
    float* data = static_cast<float*>(mxGetData(prhs[1]));
    size_t length = mxGetNumberOfElements(prhs[1]);
    
    buffer->add(data, length / buffer->getNumChannels());
}

void getData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 4) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getData", "Four input arguments required: buffer name, number of samples, channel index, and start sample.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getData", "First input argument must be a string (buffer name).");
    }
    if (!mxIsScalar(prhs[1]) || !mxIsScalar(prhs[2]) || !mxIsScalar(prhs[3])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getData", "Second, third, and fourth input arguments must be scalars (number of samples, channel index, and start sample).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    
    if (bufferMap.find(bufferName) == bufferMap.end()) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getData", "Buffer not found: %s", bufferName);
    }
    
    CircularBuffer* buffer = bufferMap[bufferName];
    size_t numSamples = static_cast<size_t>(mxGetScalar(prhs[1]));
    size_t channel = static_cast<size_t>(mxGetScalar(prhs[2]));
    size_t startSample = static_cast<size_t>(mxGetScalar(prhs[3]));
    
    plhs[0] = mxCreateNumericMatrix(1, numSamples, mxSINGLE_CLASS, mxREAL);
    float* data = static_cast<float*>(mxGetData(plhs[0]));
    
    buffer->get(data, numSamples, channel, startSample);
}

void getMostRecentData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 2) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getMostRecentData", "Two input arguments required: buffer name and number of samples.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getMostRecentData", "First input argument must be a string (buffer name).");
    }
    if (!mxIsScalar(prhs[1])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getMostRecentData", "Second input argument must be a scalar (number of samples).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    
    if (bufferMap.find(bufferName) == bufferMap.end()) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:getMostRecentData", "Buffer not found: %s", bufferName);
    }
    
    CircularBuffer* buffer = bufferMap[bufferName];
    size_t numSamples = static_cast<size_t>(mxGetScalar(prhs[1]));
    
    plhs[0] = mxCreateNumericMatrix(buffer->getNumChannels(), numSamples, mxSINGLE_CLASS, mxREAL);
    float* data = static_cast<float*>(mxGetData(plhs[0]));
    
    buffer->getMostRecent(data, numSamples);
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs < 1 || !mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:invalidInput", "First input should be a command string.");
    }
    
    char command[64];
    mxGetString(prhs[0], command, sizeof(command));
    
    if (strcmp(command, "create") == 0) {
        createBuffer(nlhs, plhs, nrhs - 1, prhs + 1);
    } else if (strcmp(command, "add") == 0) {
        addData(nlhs, plhs, nrhs - 1, prhs + 1);
    } else if (strcmp(command, "get") == 0) {
        getData(nlhs, plhs, nrhs - 1, prhs + 1);
    } else if (strcmp(command, "getMostRecent") == 0) {
        getMostRecentData(nlhs, plhs, nrhs - 1, prhs + 1);
    } else {
        mexErrMsgIdAndTxt("MATLAB:mexCircularBuffer:unknownCommand", "Unknown command.");
    }
}
