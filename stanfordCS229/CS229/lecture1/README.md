# General notes   

What machine learning heavily relies on\:   
* Convex optimization 
* Hidden Markov models  
* Times series

# Machine Learning Definition

* Arthur Samuel(1959)   
Machine learning is a field of study that gives computers ***the ability to learn without being explicitly programmed***   



When you choose a ***narrow task*** like speech recognition -- maybe you can surpass human level performance;   
playing the game of go -- by throwing tons of ***computation power*** at it and ***self play*** you can have a computer be ***very good at these narrow tasks***    

## Supervised Learning  

Given a data set with inputs X and labels Y, find a mapping from X to Y, which predicts the ys;   

* There a many ways to learn a mapping 
    * One is linear regression  
    * Or maybe you want to fit a quadratic function  

❗This is called a ***regression*** problem  
**The value Y you are trying to predict is continuous**   
* E.g. given the size of a house map that to its price

(How do you choose among different models, either automatically or manually)  

❗When **Y** taken on only a **discrete number of values**, i.e. it is a discrete random variable, the problem is called a ***classification*** problem   
* E.g.  given the size of a breast cancer tumor, map that to {0, 1} (whether or not it is malignant) 


***X might be a random vector, rather than a single variable.***  

* Logistic regression algorithm -- separate different categories  

* Support Vector Machine algorithm -- uses an infinite number of features  


## Unsupervised learning 

Given inputs X, and no Y, find some patterns, structures in this data, in this input     


Clustering, having data and figuring out what groups belong together   




