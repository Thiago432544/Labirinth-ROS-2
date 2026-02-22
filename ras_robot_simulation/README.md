# RAS Robot Simulation

Este repositório contém um pacote de ROS 2 da simulação, utilizando o Gazebo Classic, de um simples robô de duas rodas e quatro sensores de distância, cujo objetivo é chegar ao final de um labirinto de forma autônoma.

## Como utilizar

### Para compilar:

1. Clone este repositório dentro da pasta `src` do seu workspace ROS 2:

```bash
cd ~/ros2_ws/src
git clone https://github.com/RAS-UFPB/robot_ras_ros.git
```

2. Compile o workspace:

```bash
cd ~/ros2_ws
colcon build 
```

3. Source o setup:

```bash
source install/setup.bash
```

### Simulação:

Para executar a simulação:

```bash
ros2 launch robot_ras_ros gazebo.launch.py
```
