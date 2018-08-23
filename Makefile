CXXFLAGS += -std=c++17

fms_sequence.t: fms_sequence.t.o
		$(CXX) -o $@ $^ $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
