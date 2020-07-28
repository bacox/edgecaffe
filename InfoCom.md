## Experiments for InfoCom

### Batch

Variables:
* Memory: [2G, 1G, 512M]
* Number of arrivals: 150


Experiments: 
* 3 networks all small (AgeNet, GenderNet, tiny_yolo)
* 3 networks mixed in size (AgeNet, FaceNet, ObjectNet)
* 3 networks all large (ObjectNet, EmotionNet, MemNet)
* All available networks (AgeNet, GenderNet, tiny_yolo, FaceNet, ObjectNet, EmotionNet, MemNet, SceneNet, SoS_AlexNet)
* 5 networks mixed (realistic scenario, lifelogging): (tiny_yolo, EmotionNet, MemNet, SceneNet, SoS_AlexNet)

### Stochastic
Variables:
* Memory: [2G, 1G, 512M]
* Number of arrivals: 150
* Traffic intensity: [Light , normal, Heavy]

Experiments:
* realistic scenario
* Full random arrival
* 3 networks with random compositions