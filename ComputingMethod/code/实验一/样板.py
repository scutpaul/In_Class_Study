import numpy as np 

def yt_base(X,Y,type,other_param=None):
    n =  Y.shape[0]-1
    #print(X,n)
    alpha = np.zeros(n+1)
    beta = np.zeros(n+1)
    a = np.zeros(n+1)
    b = np.zeros(n+1)
    #print(alpha,beta)
    # m
    m = np.zeros(n+2)
    # h 
    h = X[1:]-X[:n]
    #unzip
    #other_param:0 m0,1 mn
    if type == 1:
        m[0] = other_param[0]
        m[n] = other_param[1]
        alpha[0] = 0
        alpha[n] = 1
        beta[0] = 2*m[0]
        beta[n] = 2*m[n]
    else:
        alpha[0] = 1
        alpha[n] = 0
        beta[0] = 3/h[0]*(Y[1]-Y[0])
        beta[n] = 3/h[n-1]*(Y[n]-Y[n-1])
    for i in range(1,n):
        alpha[i] = h[i-1]/(h[i-1]+h[i])
        beta[i] = 3*((1-alpha[i])/h[i-1]*(Y[i]-Y[i-1])+alpha[i]/h[i]*(Y[i+1]-Y[i]))
    a[0] = -alpha[0]/2
    b[0] = beta[0]/2
    for i in range(1,n+1):
        a[i] = -1*alpha[i]/(2+(1-alpha[i])*a[i-1])
        b[i] = (beta[i] - (1-alpha[i])*b[i-1])/(2+(1-alpha[i])*a[i-1])
    m[n+1] = 0
    for i in range(n,-1,-1):
        m[i] = a[i]*m[i+1] + b[i]
    return m

def yt(X,Y,x,type,other_param=None):
    M = yt_base(X,Y,type,other_param)
    i = -1
    for x_i in X:
        if x > x_i:
            i += 1
    s = 0
    print(i)
    s += (1+2*(x-X[i])/(X[i+1]-X[i]))*((x-X[i+1])/(X[i]-X[i+1]))**2*Y[i] 
    s += (1+2*(x-X[i+1])/(X[i]-X[i+1]))*((x-X[i])/(X[i+1]-X[i]))**2*Y[i+1]
    s += (x-X[i])*((x-X[i+1])/(X[i]-X[i+1]))**2*M[i]
    s += (x-X[i+1])*((x-X[i])/(X[i+1]-X[i]))**2*M[i+1]
    print(s)


# input
def main():
    X = input('输入X 空格空开，如：1 1 1 1')
    X = X.split(' ')
    for i in range(len(X)):
        X[i] = float(X[i])
    Y = input('输入Y 空格空开，如：1 1 1 1')
    Y = Y.split(' ')
    if len(X)!= len(Y):
        print('length no match')
        exit(1)
    for i in range(len(Y)):
        Y[i] = float(Y[i])
    X = np.array(X)
    Y = np.array(Y)
    XX = input('插值点')
    XX = float(XX)
    intertype = input('插值类型: 1 or 2')
    otherparam = []
    if intertype == '1':
        m0 = input('M0 = ')
        m0 = float(m0)
        mN = input('Mn = ')
        mN = float(mN)
        otherparam.append(m0)
        otherparam.append(mN)
        intertype = 1
    elif intertype == '2':
        intertype = 2
    else:
        print("插值类型错误")
        exit(1)

    #X = np.array([1,2,4,5])
    #Y = np.array([1,3,4,2])
    yt(X,Y,XX,intertype,otherparam)

if __name__ == '__main__' :
    main()









