#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class PresetFile
{
public:
	PresetFile(String name, File xmlFile);
	~PresetFile();
	String name_;
	File xmlFile_;
};

