
CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall# -fopenmp
LDFLAGS = #-fopenmp


SOURCES = main.cpp computeBondInteractions.cpp computeAngleInteractions.cpp computeDihedralInteractions.cpp computeNonBondedInteractions.cpp computeExternalInteractions.cpp computeInteractions.cpp makeVerletList.cpp initializeTemp.cpp NVE_MD.cpp NVT_MD.cpp runNVE.cpp runNVT.cpp getCOM.cpp computeRg.cpp adjust_dt.cpp writeFiles.cpp loadConfig.cpp dampedMD.cpp runDampedMD.cpp checkForPause.cpp


OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = polymer


all: $(EXECUTABLE)
	@make clean-temp

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(SOURCES:.cpp=.d)


clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(SOURCES:.cpp=.d)

clean-temp:
	rm -f $(OBJECTS) $(SOURCES:.cpp=.d)



