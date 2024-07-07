# Circle Buffer MATLAB Package (`+buf`) #

This repository provides a Circular Buffer implementation for MATLAB using a MEX function for efficient data handling. The package is designed to be added as a submodule to your projects and can be used to interact with the Circular Buffer through calls like `buf.CircleBufferMex(...)`.

## Quick Reference ##
| Method Name | Description | Syntax |
|-------------|-------------|--------|
| [create](#create-a-buffer) | Creates a buffer with specified channels and size, initializing with zeros. | `buf.CircleBufferMex('create', 'bufferName', numChannels, bufferSize);` |
| [clear](#clear-a-buffer) | Clears a buffer and frees its memory. | `buf.CircleBufferMex('clear', 'bufferName');` |
| [add](#add-data-to-the-buffer) | Adds data to the buffer. | `buf.CircleBufferMex('add', 'bufferName', data);` |
| [get](#retrieve-data-from-a-channel) | Retrieves specific data from a channel starting at a given sample. | `buf.CircleBufferMex('get', 'bufferName', numSamples, channel, startSample);` |
| [getMostRecent](#retrieve-the-most-recent-samples) | Retrieves the most recent samples from the buffer. | `buf.CircleBufferMex('getMostRecent', 'bufferName', numSamples);` |


## Installation
You'll want to add this as a submodule to your project as the `+buf` package, as shown below.

### Add as a Submodule ###
To add this repository as a submodule to your project:

```bash
cd your_project_directory
git submodule add https://github.com/Neuro-Mechatronics-Interfaces/matlab_package__circle_buffer.git +buf
git submodule update --init --recursive
```

### Compile the MEX File ###
Open MATLAB and navigate to the directory containing the CircleBufferMex.cpp file. Run the following command to compile the MEX file:  

```matlab
mex CircleBufferMex.cpp CircularBuffer.cpp
```

## Usage ##
Once the package is added as a submodule and the MEX file is compiled, you can use the Circular Buffer in your MATLAB code with the +buf package.

### Create a Buffer ###
To create a buffer:
```matlab
buf.CircleBufferMex('create', 'buffer1', numChannels, bufferSize);
```

### Add Data to the Buffer ###
To add data to the buffer:
```matlab
data = single(rand(numChannels, dataLength));
buf.CircleBufferMex('add', 'buffer1', data(:));
```

### Retrieve Data from the Buffer ###
To retrieve the most recent samples from the buffer:  

```matlab
numSamples = 100;
retrievedData = buf.CircleBufferMex('getMostRecent', 'buffer1', numSamples);
```

### Retrieve Data from a Channel ###
To retrieve specific data from a channel:  

```matlab
channel = 1;
startSample = 0;
retrievedData = buf.CircleBufferMex('get', 'buffer1', numSamples, channel, startSample);
```

### Clearing the Buffer ###  
To clear a specific named buffer:  
```matlab
buf.CircleBufferMex('clear', 'buffer1');
```

## Example ##
Here is a complete example showing how to use the Circular Buffer:

```matlab
clear;
clc;

% Parameters
numChannels = 64;
bufferSize = 4000;
dataLength = 256;

% Create the buffer
buf.CircleBufferMex('create', 'buffer1', numChannels, bufferSize);

% Generate some data to add to the buffer
data = single(rand(numChannels, dataLength));

% Add data to the buffer
buf.CircleBufferMex('add', 'buffer1', data(:));

% Retrieve the most recent 100 samples from the buffer
numSamples = 100;
retrievedData = buf.CircleBufferMex('getMostRecent', 'buffer1', numSamples);
disp(retrievedData);
```

## License ##
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgements ##
This project is maintained by the Neuro-Mechatronics Interfaces Lab.