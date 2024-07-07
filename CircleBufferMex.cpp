#include "mex.h"
#include "CircularBuffer.h"
#include <map>
#include <string>

std::map<std::string, CircularBuffer*> bufferMap;

void createBuffer(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 3) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:createBuffer", "Three input arguments required: buffer name, number of channels, and buffer size.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:createBuffer", "First input argument must be a string (buffer name).");
    }
    if (!mxIsScalar(prhs[1]) || !mxIsScalar(prhs[2])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:createBuffer", "Second and third input arguments must be scalars (number of channels and buffer size).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    size_t numChannels = static_cast<size_t>(mxGetScalar(prhs[1]));
    size_t bufferSize = static_cast<size_t>(mxGetScalar(prhs[2]));
    
    bufferMap[bufferName] = new CircularBuffer(numChannels, bufferSize);
}

void clearBuffer(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:clearBuffer", "One input argument required: buffer name.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:clearBuffer", "First input argument must be a string (buffer name).");
    }

    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));

    auto it = bufferMap.find(bufferName);
    if (it != bufferMap.end()) {
        delete it->second;
        bufferMap.erase(it);
    } else {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:clearBuffer", "Buffer not found: %s", bufferName);
    }
}

void addData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 2) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:addData", "Two input arguments required: buffer name and data array.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:addData", "First input argument must be a string (buffer name).");
    }
    if (!mxIsSingle(prhs[1])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:addData", "Second input argument must be an array of singles (data).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    
    if (bufferMap.find(bufferName) == bufferMap.end()) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:addData", "Buffer not found: %s", bufferName);
    }
    
    CircularBuffer* buffer = bufferMap[bufferName];
    float* data = static_cast<float*>(mxGetData(prhs[1]));
    size_t length = mxGetNumberOfElements(prhs[1]);
    
    buffer->add(data, length / buffer->getNumChannels());
}

void getData(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs != 4) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getData", "Four input arguments required: buffer name, number of samples, channel index, and start sample.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getData", "First input argument must be a string (buffer name).");
    }
    if (!mxIsScalar(prhs[1]) || !mxIsScalar(prhs[2]) || !mxIsScalar(prhs[3])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getData", "Second, third, and fourth input arguments must be scalars (number of samples, channel index, and start sample).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    
    if (bufferMap.find(bufferName) == bufferMap.end()) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getData", "Buffer not found: %s", bufferName);
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
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getMostRecentData", "Two input arguments required: buffer name and number of samples.");
    }
    if (!mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getMostRecentData", "First input argument must be a string (buffer name).");
    }
    if (!mxIsScalar(prhs[1])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getMostRecentData", "Second input argument must be a scalar (number of samples).");
    }
    
    char bufferName[128];
    mxGetString(prhs[0], bufferName, sizeof(bufferName));
    
    if (bufferMap.find(bufferName) == bufferMap.end()) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:getMostRecentData", "Buffer not found: %s", bufferName);
    }
    
    CircularBuffer* buffer = bufferMap[bufferName];
    size_t numSamples = static_cast<size_t>(mxGetScalar(prhs[1]));
    
    plhs[0] = mxCreateNumericMatrix(buffer->getNumChannels(), numSamples, mxSINGLE_CLASS, mxREAL);
    float* data = static_cast<float*>(mxGetData(plhs[0]));
    
    buffer->getMostRecent(data, numSamples);
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs < 1 || !mxIsChar(prhs[0])) {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:invalidInput", "First input should be a command string.");
    }
    
    char command[64];
    mxGetString(prhs[0], command, sizeof(command));
    
    if (strcmp(command, "create") == 0) {
        createBuffer(nlhs, plhs, nrhs - 1, prhs + 1);
    } else if (strcmp(command, "clear") == 0) {
        clearBuffer(nlhs, plhs, nrhs - 1, prhs + 1);
    } else if (strcmp(command, "add") == 0) {
        addData(nlhs, plhs, nrhs - 1, prhs + 1);
    } else if (strcmp(command, "get") == 0) {
        getData(nlhs, plhs, nrhs - 1, prhs + 1);
    } else if (strcmp(command, "getMostRecent") == 0) {
        getMostRecentData(nlhs, plhs, nrhs - 1, prhs + 1);
    } else {
        mexErrMsgIdAndTxt("MATLAB:CircleBufferMex:unknownCommand", "Unknown command.");
    }
}
