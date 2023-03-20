# Processing continuous audio
An oscillator in JUCE (or any other digital audio system) needs to know the previous sample value in order to calculate the next sample value.

When an oscillator generates a waveform, it uses an internal phase accumulator to keep track of its position within the waveform. The phase accumulator is updated on each call to the `renderNextBlock()` function, and determines the position of the oscillator within the waveform.

To generate the next sample value, the oscillator reads the sample value at the current position in the waveform, and uses **linear interpolation** to calculate the exact value of the waveform between samples. The oscillator then increments its phase accumulator by the desired frequency, and repeats the process for the next sample.

A phase accumulator is a digital signal processing technique used in audio synthesis to keep track of the phase of an oscillating waveform.

In an oscillator, the phase of the waveform determines the position of the waveform over time. The phase accumulator is a variable that is updated on each sample, and represents the current phase of the oscillator. The value of the phase accumulator is typically represented as a fractional index into the waveform, with a range of 0 to 1 representing a complete cycle of the waveform.

The phase accumulator is used to calculate the next sample value for the oscillator by determining the current position within the waveform and interpolating between the adjacent samples. The frequency of the oscillator is determined by the rate at which the phase accumulator is incremented, which is typically a function of the desired frequency and the sample rate of the audio stream.
#### Phase Accumulator  

The phase accumulator can be thought of as a pointer that moves through a lookup table of precomputed waveform values. On each sample, the accumulator is incremented by a value proportional to the desired frequency, and the waveform value at the current accumulator position is read from the lookup table. The accumulator is then used to interpolate between the adjacent waveform values to determine the exact value of the waveform at the current sample.







# juce::SynthesizerVoice
The `juce::SynthesiserVoice` class is a base class for creating voices for a synthesizer in JUCE. It contains methods that are common to all types of synthesizer voices, such as triggering and stopping the voice, handling pitch bend and aftertouch, and rendering the audio for a particular note. When creating a new synthesizer in JUCE, you would typically subclass `juce::SynthesiserVoice` to create your own custom voice class that implements the specifics of your synthesizer.

would it make sense to have a rompler or sampler voice subclass this class? For midi etc?

Yes, it would make sense to have a rompler or sampler voice subclass the `juce::SynthesiserVoice` class for MIDI. The `SynthesiserVoice` class is designed to provide the base functionality for a voice in a synthesizer, handling the allocation, deallocation, and MIDI event processing for each voice. Since a sampler or rompler is also a type of synthesizer, it would make sense to subclass `SynthesiserVoice` to handle the MIDI event processing and voice allocation.

# juce renderNextBlock method (and kind of a rabbithole of audio callbacks)
`renderNextBlock` is a method in the `SynthesiserVoice` class of JUCE that generates the audio output for the given `AudioBuffer`. It takes two parameters - the `outputBuffer` and the `startSample` - and fills the buffer with audio samples for the requested block.

When `renderNextBlock` is called, it calculates the audio samples for the block by calling the `renderNextBlock` method of the underlying audio source, such as an oscillator, sampler, or wavetable.

The purpose of `renderNextBlock` is to generate audio output for a voice, which is then combined with the audio output of other voices by the `Synthesiser` class to produce the final audio output for the synthesizer.

Question: So where and when is `renderNextBlock` called in relation to `PluginProcessor::processBlock`?

`renderNextBlock` is typically called by `PluginProcessor::processBlock` for each audio callback. 

Question: It sounds like what we're doing in these `*Block` methods is simply moving from sample to sample (probably in groups based on the host's block size) with the goal of completing a loop through the audio buffer. Is the reason we see references to "nextBlock" in some of the juce code because our current block sort of determines the next block and the current block is sort of determined by the last? Do we need to do this in order to preserve some form of continuity? Or is it more for speed / real time concerns?

Yes! The `*Block` methods are called repeatedly to process blocks of audio samples. The block size is typically determined by the host, and is usually a power of two (e.g. 64, 128, 256, 512, 1024, etc.).

The reason why we see references to "nextBlock" in some of the JUCE code is that the output of the current block can depend on the input of the next block. This is known as "lookahead". For example, some effects might need to analyze the audio signal ahead of time in order to determine how to process the current block. In order to do this, they might need to process the next block as well, which is why you might see references to "nextBlock" in their code.

The reason why we process audio in blocks like this is mainly for performance reasons. Processing audio in real-time requires a lot of computational power, and breaking it up into blocks allows us to perform the processing in smaller, more manageable chunks. Additionally, many audio processing algorithms are optimized for processing data in blocks, rather than on a per-sample basis.

The `processBlock` method is responsible for calling the `renderNextBlock` method *for each voice* to generate audio samples, and then combining these samples to produce the final output audio buffer. The `renderNextBlock` method is therefore a key component of the voice's audio generation process and is called repeatedly during audio processing.


# juce Component Name args in constructor

The component name string provided in the `juce::Label` constructor is used to identify the component within the parent component's hierarchy. This is important because it allows the component to be found and manipulated programmatically using functions like `findChildWithID` or `getChildComponent`. The name is also used for debugging purposes, as it can be displayed in error messages or logging output to help identify the component that caused the issue.



# juce::PluginProcessor
In JUCE, the PluginProcessor class is the central component of an audio plugin. It is responsible for processing audio and managing the plugin's parameters.

When an audio plugin is loaded, the host application creates an instance of the PluginProcessor class. The PluginProcessor is responsible for processing audio data in real-time, using the plugin's internal state and parameters. It receives audio input from the host application, processes it, and sends the output back to the host.

The PluginProcessor class also manages the plugin's parameters. It defines the parameters that can be controlled by the user, and provides methods for reading and modifying their values. It communicates with the GUI using the AudioProcessorEditor class, which is responsible for displaying the plugin's parameters and allowing the user to modify them.

The PluginProcessor class is an abstract class, and it must be subclassed to create a plugin. The subclass must implement the processBlock method, which is called by the host application to process audio data. The subclass can also define additional methods and member variables as needed to implement the plugin's functionality.


# juce::PluginEditor

In JUCE, the AudioProcessorEditor class is used to create the graphical user interface (GUI) for an audio plugin, while the AudioProcessor class handles the audio processing functionality.

The AudioProcessorEditor class is a subclass of the JUCE Component class and provides a canvas on which to draw the plugin's GUI. The AudioProcessorEditor is created by the AudioProcessor and is responsible for creating and managing all of the GUI components of the plugin.

The AudioProcessorEditor class typically includes a number of GUI components, such as sliders, buttons, and text fields, that are used to control the plugin's parameters and display its state. These components are created and added to the AudioProcessorEditor in the class constructor, using JUCE's built-in UI components and layout managers.

Once the AudioProcessorEditor is created, it is added to the audio plugin's window by the AudioProcessor class, and the GUI is displayed to the user. The user can then interact with the GUI components to control the audio processing behavior of the plugin.

# juce Audio Processor Value Tree State
### TLDR its a tree with change notification listeners / broadcasters so it connects the UI and Data in a very immediate nice way.

In JUCE, the AudioProcessorValueTreeState class is a powerful tool for managing the state of a plugin's parameters. It provides a way to create, manage, and synchronize the state of parameters between the GUI and the audio processing code.

The AudioProcessorValueTreeState class is based on a hierarchical data structure called a ValueTree. Each parameter in the plugin is represented as a ValueTree node, and the state of each parameter is stored in the properties of that node.

One of the main advantages of using AudioProcessorValueTreeState is that it automatically handles the communication between the GUI and the audio processing code. When a parameter is changed in the GUI, the AudioProcessorValueTreeState updates the corresponding ValueTree node, which then triggers a change notification. The audio processing code can then respond to the change by updating its internal state.

The AudioProcessorValueTreeState also provides a convenient way to manage the state of a plugin across multiple instances, such as when the plugin is hosted in a DAW. The state of the plugin is saved as a ValueTree and can be easily loaded and saved.

Overall, the AudioProcessorValueTreeState is a powerful and flexible tool for managing the state of a plugin's parameters, and it is widely used in JUCE-based audio plugins.


### juce::AudioProcessorValueTreeState::ParameterLayout 

In JUCE, the AudioProcessorValueTreeState::ParameterLayout class is used to define the layout of a plugin's parameters. It is used to create and manage the parameters of a plugin, and to map those parameters to a ValueTree node.

The ParameterLayout class provides a way to define the parameters of a plugin and their properties, such as name, label, default value, and range. It also provides methods for adding and removing parameters, and for mapping them to a ValueTree node.

By using ParameterLayout, developers can easily define the parameters of a plugin in a structured way, rather than managing them directly through code. This allows for more flexible and modular code, as well as easier parameter management across different platforms and hosts.

The ParameterLayout class is often used in conjunction with AudioProcessorValueTreeState, which manages the state of a plugin's parameters using a ValueTree data structure. By defining the parameters using ParameterLayout and mapping them to a ValueTree node, developers can easily manage the state of their plugin's parameters and keep the GUI and audio processing code in sync.

## The Glue of GUI and DATA (attachments)

In JUCE, ValueTreeState::Listener objects can use ValueTreeState::Attachment objects to easily connect and synchronize values between a ValueTree and a component in the plugin's GUI.

A ValueTreeState::Attachment object can be created for a ValueTree property and a GUI component, such as a slider or button. The Attachment object listens for changes to the ValueTree property and updates the GUI component accordingly, and also listens for changes to the GUI component and updates the ValueTree property.

For example, the following code creates a Slider component and attaches it to a ValueTree property using a ValueTreeState::Attachment object:

```cpp
auto slider = std::make_unique<Slider>(Slider::RotaryVerticalDrag, Slider::TextBoxBelow);
addAndMakeVisible(slider.get());

auto& valueTreeState = getAudioProcessorValueTreeState();
auto attachment = std::make_unique<ValueTreeState::SliderAttachment>(valueTreeState, "myParam", *slider);
```
In this example, the Slider component is created and added to the plugin's GUI, and an AudioProcessorValueTreeState object is retrieved. The ValueTreeState::SliderAttachment object is then created, passing in the AudioProcessorValueTreeState object, the name of the ValueTree property to attach to, and the Slider component.

Once the attachment is created, the Slider component will automatically synchronize its value with the ValueTree property, and any changes made to the Slider will be reflected in the ValueTree property.

Overall, ValueTreeState::Attachment objects provide a simple and convenient way to synchronize values between a ValueTree and a component in the plugin's GUI, and they are a powerful tool for building flexible and responsive plugins in JUCE.


# juce::dsp::ProcessSpec

The `juce::dsp::ProcessSpec` class in JUCE is used to encapsulate the specifications or parameters of an audio processing operation. It provides information about the sample rate, the number of input and output channels, and the maximum number of samples that may be processed in a single call.

This class is used extensively in the JUCE DSP module to specify the processing requirements of various audio processing algorithms, such as filters, FFTs, and delay lines. The ProcessSpec object is typically passed as an argument to the `prepare()` method of a processing algorithm to initialize its internal state based on the given parameters.

Here is a brief overview of the member variables of the juce::dsp::ProcessSpec class:

`sampleRate:` The sample rate of the audio being processed, in Hertz.

`maximumBlockSize:` The maximum number of samples that may be processed in a single call to the algorithm.

`numChannels:` The number of input and output channels of the audio being processed.

`options:` A bitfield of options that may affect the behavior of the algorithm, such as whether to perform in-place processing or allocate temporary buffers.

By encapsulating the processing specifications in a single object, the ProcessSpec class helps to simplify the interface of audio processing algorithms and make them more modular and reusable.

## prepareToPlay

The prepareToPlay method in JUCE is called by the audio device manager just before the audio stream starts playing. It is typically used to perform any necessary setup operations for the audio processing, such as initializing buffers, calculating filter coefficients, or allocating memory for processing audio.

In prepareToPlay, you typically initialize any resources that need to be initialized before audio playback begins, such as allocating memory or setting up data structures. You can also initialize audio buffers or audio processing objects that need to be reset before audio playback starts.

You would pass process spec to prepare to play among other things and not exclusive to processSpec. Sample rate and buffer size for instance.


<!--end of juce notes-->
------------------------------------
# C++ 


# Destructors

The ~OscComponent is the destructor of the OscComponent class.

In C++, a destructor is a special member function that is automatically called when an object of a class is destroyed. The purpose of the destructor is to perform any necessary cleanup operations before the object is deallocated from memory.

In the case of the OscComponent class, the destructor is defined using the override keyword to indicate that it overrides the base class destructor. The destructor does not include any additional functionality, so it simply performs the default cleanup operations required by the base class destructor.

In general, it is good practice to define a destructor for a class if the class has any non-trivial cleanup operations that need to be performed when the object is destroyed. In the case of the OscComponent class, the destructor is provided to ensure that the object is properly cleaned up when it is destroyed, even if no additional cleanup operations are required.

# Preprocessor Macros

When you define a macro, the compiler replaces all instances of the macro name with the macro's expansion code. This is done before the code is compiled, so the resulting code is effectively the same as if you had manually written the expanded code yourself.

Example

```cpp
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```

At compile time:

```cpp
int x = 5;
int y = 10;
int z = MAX(x, y);  // expands to: int z = ((x) > (y) ? (x) : (y));
```

## JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR is a preprocessor macro in JUCE that defines a set of functions and member variables to prevent copying and assignment of a class, and to detect memory leaks in the class.

The macro defines two private member functions for the class: a copy constructor and an assignment operator, both of which are declared but not defined. This prevents objects of the class from being copied or assigned, since attempting to do so will result in a linker error.

The macro also defines a LeakedObjectDetector member variable for the class, which provides a mechanism for detecting memory leaks in the class. The LeakedObjectDetector class uses a static counter to keep track of the number of instances of the class that have been created, and reports a warning if any instances have not been properly destroyed when the program exits.


# Initializer Lists

An initializer list is a C++ feature that allows you to initialize the member variables of a class or struct using a concise syntax.

In an initializer list, the member variables are listed in curly braces {} after the class or struct name, separated by commas. For example:

```cpp
struct MyStruct {
int a;
float b;
double c;

MyStruct(int x, float y, double z) : a(x), b(y), c(z) {}
};
```

In this example, the MyStruct struct has three member variables: a, b, and c. The constructor for MyStruct takes three arguments, which are used to initialize the member variables using an initializer list.

The initializer list syntax is used in the constructor definition, after the colon `:`. Each member variable is initialized by specifying its name, followed by the value to initialize it with, separated by parentheses `()`.

Initializer lists can be a more concise and efficient way to initialize member variables, especially for complex or compound types such as arrays or objects. They can also be used to ensure that all member variables are properly initialized before the constructor body is executed.

Overall, initializer lists are a powerful and useful feature in C++, and are commonly used to initialize the member variables of classes and structs in a concise and efficient way.


# Uniform initialization

`int lastMidiNote { 0 };`

It just prevents narrowing. For example

`short x {50000}` would throw a commpiler error bc 50000 cant be a short 

whereas `short x = 50000` would just shave the number down silently.


# STD::UniquePointer

In C++, std::unique_ptr is a smart pointer that provides automatic memory management for dynamically allocated objects.

When you use std::unique_ptr to create a new object, the memory for that object is automatically allocated on the heap, and is managed by the smart pointer. The smart pointer ensures that the memory is properly deallocated when it is no longer needed, which helps to prevent memory leaks and other memory-related bugs.

One of the key features of std::unique_ptr is that it provides "ownership semantics" for the object it points to. This means that only one std::unique_ptr can own a particular object at any given time, and that ownership can be transferred between std::unique_ptr instances using move semantics.

For example, consider the following code:

```cpp
std::unique_ptr<MyClass> ptr1(new MyClass(10));
std::unique_ptr<MyClass> ptr2 = std::move(ptr1);
```

In this example, two std::unique_ptr instances (ptr1 and ptr2) are created to manage a new instance of the MyClass object. The std::unique_ptr instance ptr1 is initially responsible for managing the memory for the object, but ownership is transferred to ptr2 using move semantics.

Overall, std::unique_ptr is a powerful and useful feature in C++, and is commonly used to manage dynamically allocated objects in a safe and efficient way. By providing automatic memory management and ownership semantics, std::unique_ptr helps to reduce bugs and make code more robust and reliable.

# Anonymous functions in c++

In C++, anonymous functions are called lambda expressions, and they can be created using the lambda syntax.

A lambda expression is a way to define a small function inline, without having to define a separate named function. Lambda expressions can be useful in situations where you need to define a small function for a specific purpose, such as passing a function as an argument to another function or performing a short computation.

The general syntax of a lambda expression is:

```cpp
[capture-list] (parameter-list) -> return-type { body }
```
where:

`capture-list` is an optional list of variables that the lambda expression can capture from its enclosing scope
`parameter-list` is a comma-separated list of parameters to the lambda function
`return-type` is an optional return type for the lambda function
`body` is the code to be executed when the lambda function is called

For example, the following lambda expression defines a simple function that takes two integers and returns their sum:
```cpp
auto sum = [](int a, int b) { return a + b; };
```










# END END END
--------------------------------------
### LINKS TO LEARNING

Designing a straightforward limiter
https://www.youtube.com/watch?v=ByU_n1qucXg

DSP Lecture series (not sure if its good)
https://www.youtube.com/watch?v=hVOA8VtKLgk

Audio Processing in Faust
part 1 https://www.youtube.com/watch?v=LHLlWNq5TKk
part 2 https://www.youtube.com/watch?v=mF_EQ4sIE1w
part 3 https://www.youtube.com/watch?v=9liX9Jf7RkE


# XCODE shortcuts

#### Auto format
`command + a`
`ctrl + i`
#### Select next instance

`Command + option + e` 

#### Switch tabs
`Command + shift {` left
`Command + shift }` right

