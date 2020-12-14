## Experiments for InfoCom

### Batch

Variables:
* Memory: [2G, 1G, 512M]
* Number of arrivals: 150


Experiments: 
* 3 networks all small (AgeNet, GenderNet, tiny_yolo)
* 3 networks mixed in size (AgeNet, FaceNet, EmotionNet)
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


### Building on RPI
```bash
mkdir -p cmake-build-release
cd ~/edgecaffe/cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
cpack
```

### Installing on RPI

* install `EdgeCaffe-1.x.x-Linux-EdgeCaffeApp`
    `sudo apt-get install EdgeCaffe-1.3.0-Linux-EdgeCaffeApp`
* Make sure the (splitted) models are copied to the pi. `scrips/copy_models.sh` can be used.
* Make sure that a swapfile exists:
```bash
sudo swapoff -a
sudo dd if=/dev/zero of=/swapfile bs=1G count=8
sudo mkswap /swapfile
sudo swapon /swapfile
sudo swapon -a
```
* Make sure the folders are created and the permissions are correct.
    ```bash
    sudo mkdir -p /opt/analysis/thermal
    sudo chown -R $USER:$USER /opt/edgecaffe
  sudo chown -R $USER:$USER /opt/analysis
  sudo chmod 755  /opt/edgecaffe/thermal/record.sh
  sudo systemctl daemon-reload 
    ```
 ### Running experiments on RPI
 * Make sure the swapfile is enabled:
 ```bash
sudo swapoff -a && sudo mkswap /swapfile && sudo swapon /swapfile && sudo swapon -a
 ```

* Run the experiments: 
```bash
cd /opt/edgecaffe 
sudo nohup python3 scripts/infocom/exp_runner.py --mem-limit 512M|1G|2G --logging true &
```