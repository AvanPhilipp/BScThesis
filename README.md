# BSc Thesis
All the codes of my BSc Thesis are stored here. The purpose is to secure all the data for further presentation.
## credits:
The base of this project is the convolutional network implementation of Dr. Zoltán Nagy. The files *"conv.cpp"*, *"conv.h"*, *"conv_tb.cpp"* is his work.
I started the work with the fully connected layer implementation.
#Files
1. mnist.py
... TensorFlow tutorial for deep learning. Its basically not important what or how i train on the FPGA but is to implement the trained network on the IC. The mnist list is the perfect for this. small easy to train and many more did it before me. if I can make it work on an FPGA than any other deep learning system is possible.
2. MNIST_data is the containing folder of the mnist dataset.
* trainingset 55000 datapoint
* testdata 10000 datapoint
* validation data 5000 point

### TODO
- [x] Súlyok egy interfacen végigvezetve
- [x] TestBranch
- [x] Fully Connected bitenként adja ki az eredményt
- [ ] Sythesis
- [ ] Implemetácio
- [ ] Apa: "Tensorflow" ezt kis- vagy nagy betűvel kell írni? Már magam sem tudom. Végig kellene menni a szövegen és egységesíteni!

### Questions
- [ ] SegFault valahol a kódba. Conv_net sum[0] = weight\*input
- [ ] Conv\_templ végén address = 0 ha IN\_WIDTH-3??? IN\_WIDTH-TEMPLATE\_Size
- [ ] Address új sorban nem jó(?) értékekkel dolgozik ???
- [ ] Az MNIST MSB LSB problémája mennyire okoz gondot.
- [ ] Tensorflow nem ugyan azt húzza be ???