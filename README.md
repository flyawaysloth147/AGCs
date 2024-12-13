# Axantera Ground Control

## Table of Contents

- [Project Name](#project-name)
  - [Table of Contents](#table-of-contents)
  - [Description](#description)
  - [Features](#features)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Contributing](#contributing)
  - [License](#license)
  - [Acknowledgements](#acknowledgements)
  - [Contact](#contact)

## Description

**Axantera Ground Control** is a software made by the Axantera rocket team, the purpose of this software is to visualy monitor the rocket data such as altitude and rotation, the software fetch the data through selected serial port and parse it for showing it to the screen.

## Features

- Visualy shows realtime data
- Connect to serial port easily
- Simple yet informative UI

## Installation

Instructions on how to install and set up the project.

```bash
# Clone the repository
git clone --recursive https://github.com/flyawaysloth147/AGCs.git 
```

## Usage

**You can see the APIs for the software here in the tables below.**

Note all of the variable will be float

| Data number(int)  | Data Type                  | Format (example)     |
| -------------     | -------------              | ----------           |   
| 1                 | Rotation(Roll, Pitch, Yaw) | 1 2.0 24.5 23.0      |   
| 2                 | Pressure                   | 2 32.0               |   
| 3                 | Temprature                 | 3 49.0               |   
| 4                 | Altitude                   | 4 32.0               |   
| 5                 | Apogee                     | 5 50.0               |   
| 6                 | Payload seperated          | 6                    |   
| 7                 | Parachute Deployed         | 2                    |   

```c++
// This is the example for the arduino code to send data
// Rotation
Serial.print("1 ");
Serial.print(Roll); Serial.print(" ");
Serial.print(Pitch); Serial.print(" ");
Serial.print(Yaw);



```

## Contributing

If you want to contribute to this project, please follow these steps:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes.
4. Commit your changes (`git commit -m 'Add some feature'`).
5. Push to the branch (`git push origin feature-branch`).
6. Create a new Pull Request.

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for more details on the code of conduct, and the process for submitting pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

- [Person or Project](link) - for inspiration, code snippets, etc.

## Contact

Your Name - [your.email@example.com](mailto:your.email@example.com)

Project Link: [https://github.com/yourusername/your-repo-name](https://github.com/yourusername/your-repo-name)