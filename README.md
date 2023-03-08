# Plugin Processor
In JUCE, the PluginProcessor class is the central component of an audio plugin. It is responsible for processing audio and managing the plugin's parameters.

When an audio plugin is loaded, the host application creates an instance of the PluginProcessor class. The PluginProcessor is responsible for processing audio data in real-time, using the plugin's internal state and parameters. It receives audio input from the host application, processes it, and sends the output back to the host.

The PluginProcessor class also manages the plugin's parameters. It defines the parameters that can be controlled by the user, and provides methods for reading and modifying their values. It communicates with the GUI using the AudioProcessorEditor class, which is responsible for displaying the plugin's parameters and allowing the user to modify them.

The PluginProcessor class is an abstract class, and it must be subclassed to create a plugin. The subclass must implement the processBlock method, which is called by the host application to process audio data. The subclass can also define additional methods and member variables as needed to implement the plugin's functionality.


# Plugin Editor

In JUCE, the AudioProcessorEditor class is used to create the graphical user interface (GUI) for an audio plugin, while the AudioProcessor class handles the audio processing functionality.

The AudioProcessorEditor class is a subclass of the JUCE Component class and provides a canvas on which to draw the plugin's GUI. The AudioProcessorEditor is created by the AudioProcessor and is responsible for creating and managing all of the GUI components of the plugin.

The AudioProcessorEditor class typically includes a number of GUI components, such as sliders, buttons, and text fields, that are used to control the plugin's parameters and display its state. These components are created and added to the AudioProcessorEditor in the class constructor, using JUCE's built-in UI components and layout managers.

Once the AudioProcessorEditor is created, it is added to the audio plugin's window by the AudioProcessor class, and the GUI is displayed to the user. The user can then interact with the GUI components to control the audio processing behavior of the plugin.

#  Audio Processor Value Tree State
### TLDR its a tree with change notification listeners / broadcasters so it connects the UI and Data in a very immediate nice way.

In JUCE, the AudioProcessorValueTreeState class is a powerful tool for managing the state of a plugin's parameters. It provides a way to create, manage, and synchronize the state of parameters between the GUI and the audio processing code.

The AudioProcessorValueTreeState class is based on a hierarchical data structure called a ValueTree. Each parameter in the plugin is represented as a ValueTree node, and the state of each parameter is stored in the properties of that node.

One of the main advantages of using AudioProcessorValueTreeState is that it automatically handles the communication between the GUI and the audio processing code. When a parameter is changed in the GUI, the AudioProcessorValueTreeState updates the corresponding ValueTree node, which then triggers a change notification. The audio processing code can then respond to the change by updating its internal state.

The AudioProcessorValueTreeState also provides a convenient way to manage the state of a plugin across multiple instances, such as when the plugin is hosted in a DAW. The state of the plugin is saved as a ValueTree and can be easily loaded and saved.

Overall, the AudioProcessorValueTreeState is a powerful and flexible tool for managing the state of a plugin's parameters, and it is widely used in JUCE-based audio plugins.


### AudioProcessorValueTreeState::ParameterLayout 

In JUCE, the AudioProcessorValueTreeState::ParameterLayout class is used to define the layout of a plugin's parameters. It is used to create and manage the parameters of a plugin, and to map those parameters to a ValueTree node.

The ParameterLayout class provides a way to define the parameters of a plugin and their properties, such as name, label, default value, and range. It also provides methods for adding and removing parameters, and for mapping them to a ValueTree node.

By using ParameterLayout, developers can easily define the parameters of a plugin in a structured way, rather than managing them directly through code. This allows for more flexible and modular code, as well as easier parameter management across different platforms and hosts.

The ParameterLayout class is often used in conjunction with AudioProcessorValueTreeState, which manages the state of a plugin's parameters using a ValueTree data structure. By defining the parameters using ParameterLayout and mapping them to a ValueTree node, developers can easily manage the state of their plugin's parameters and keep the GUI and audio processing code in sync.

# The Glue of GUI and DATA (attachments)

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

## Destructors

The ~OscComponent is the destructor of the OscComponent class.

In C++, a destructor is a special member function that is automatically called when an object of a class is destroyed. The purpose of the destructor is to perform any necessary cleanup operations before the object is deallocated from memory.

In the case of the OscComponent class, the destructor is defined using the override keyword to indicate that it overrides the base class destructor. The destructor does not include any additional functionality, so it simply performs the default cleanup operations required by the base class destructor.

In general, it is good practice to define a destructor for a class if the class has any non-trivial cleanup operations that need to be performed when the object is destroyed. In the case of the OscComponent class, the destructor is provided to ensure that the object is properly cleaned up when it is destroyed, even if no additional cleanup operations are required.

## JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR is a preprocessor macro in JUCE that defines a set of functions and member variables to prevent copying and assignment of a class, and to detect memory leaks in the class.

The macro defines two private member functions for the class: a copy constructor and an assignment operator, both of which are declared but not defined. This prevents objects of the class from being copied or assigned, since attempting to do so will result in a linker error.

The macro also defines a LeakedObjectDetector member variable for the class, which provides a mechanism for detecting memory leaks in the class. The LeakedObjectDetector class uses a static counter to keep track of the number of instances of the class that have been created, and reports a warning if any instances have not been properly destroyed when the program exits.

## Preprocessor Macros

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

## Initializer Lists

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

## juce::dsp::ProcessSpec

The `juce::dsp::ProcessSpec` class in JUCE is used to encapsulate the specifications or parameters of an audio processing operation. It provides information about the sample rate, the number of input and output channels, and the maximum number of samples that may be processed in a single call.

This class is used extensively in the JUCE DSP module to specify the processing requirements of various audio processing algorithms, such as filters, FFTs, and delay lines. The ProcessSpec object is typically passed as an argument to the `prepare()` method of a processing algorithm to initialize its internal state based on the given parameters.

Here is a brief overview of the member variables of the juce::dsp::ProcessSpec class:

`sampleRate:` The sample rate of the audio being processed, in Hertz.

`maximumBlockSize:` The maximum number of samples that may be processed in a single call to the algorithm.

`numChannels:` The number of input and output channels of the audio being processed.

`options:` A bitfield of options that may affect the behavior of the algorithm, such as whether to perform in-place processing or allocate temporary buffers.

By encapsulating the processing specifications in a single object, the ProcessSpec class helps to simplify the interface of audio processing algorithms and make them more modular and reusable.


## Uniform initialization

`int lastMidiNote { 0 };`

It just prevents narrowing. For example

`short x {50000}` would throw a commpiler error bc 50000 cant be a short 

whereas `short x = 50000` would just shave the number down silently.


## STD Unique Pointer

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

## Anonymous functions in c++

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

## XCODE shortcuts
https://medium.com/bpxl-craft/our-favorite-xcode-shortcuts-d3271c481d56#:~:text=1.,in%20scope%3A%20CTRL%2B%E2%8C%98%2BE
```

#### Select next instance

`Command + option + e` 

