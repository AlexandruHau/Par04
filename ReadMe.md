# Modeling and Visualization in Physics Exam 

## Exam number: B138848

### Instructions for running the code

The best way to run the code is from the terminal - type the following:

```
python3 B138848_MVP_Exam_2022.py
```
The following message will appear immediately after running the code:

```
ok = int(input("Does the user prefer the data saved for part (B)? Choose (0) if you wish to proceed with the other subpoints"))
```

For the best performance of the simulation, it is most suitable if the 0-value is introduced in cae the points A,C,E or F are run. The 1-value should be introduced only for point B, as the function for B is implemented in the part C.

### Question 1 a)

### Question 1 b)

Initially, the system is dominated by the 0-state, when neither a, nor b or c yield 
high values. After a short time, however, the 0-state is quickly destroyed, and the states 1, 2 and 3 co-exist until one of them eventually destroys the others. Afterwards, the system reaches an absorbing state. A screenshot is provided with the system update of the three chemical species, where two of them are annihilated and one completely dominates. 

![ScreenShot](300GeV.png)

### Question 1 c)

Mean lifetime of a system is: 1641.5 time steps and the error on the lifetime calculating through the error on the mean formula is: 310.922 time steps 

### Question 1 d)

For the new set of parameters, the 0-state is quickly killed and the states 1, 2 and 3 infinitely co-exist.

### Question 1 e)

The plot with the peaks for the two points is provided below: this is done for simulation of 5000 timesteps, for the new set of parameters for D, p and q. The periods are approximately similar, with 15 peaks in 5000 time steps, yielding a time period of $\frac{5000}{15} = 333$ seconds.

![ScreenShot](300GeV_G4_ONNX.png)

### Question 1 f)

The two-point correlation function has been analysed as function of distance. Firstly, one random point has been taken. Afterwards, points at the distances 1,2,..,25 on x-axis have been taken wrt the original point. For **each** of these distances, a simulation of 1000 time steps has been conducted and the probability of two cells being the same has been computed.

The results are shown below: for D = 0.5 yield negative correlation coefficient of -0.56 and for D = 0.3 yield negative correlation coefficient of -0.69

![ScreenShot](G4_global_coords.png)