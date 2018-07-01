import numpy  as np
def check(x,xita):
    if abs(x) <= xita:
        print("failed")
        exit(0)
def Jacobi(_A,b,xita,M):
    A = _A.copy()
    A = A.astype(np.float)
    print(A.dtype)
    n = A.shape[0] #矩阵的行
    k = 1 #迭代次数
    Y = np.zeros(n) #保存上次迭代结果
    g = np.zeros(n)
    X = Y.copy()
    print(X,Y.dtype)
    # 获取B g
    for i in range(n):
        check(A[i,i],xita)
        T = A[i,i]
        for j in range(n):
            A[i,j] = -1*A[i,j]/T
        A[i,i] = 0
        g[i] = b[i]/T
    print('B:',A)
    print('g:',g)
    # 迭代过程
    while 1:
        for i in range(n):
            X[i] = g[i]+np.sum(A[i,:]*Y)-A[i,i]*Y[i]
        if np.sum(np.square(X-Y))<xita:
            print(X,k)
            break
        elif k>=M:
            print('超过次数',X,k)
            break
        k += 1
        Y = X.copy()#copy!!!!!!!!!!

def main():
    n = input("输入n")
    n = int(n)
    A = []
    for i in range(n):
        temp = input('输入X'+str(i+1)+'行 空格空开，如：1 1 1 1')
        temp = temp.split(' ')
        for j in range(n):
            temp[j] = float(temp[j])
        A.append(temp)
    A = np.array(A)
    b = input('输入b:')
    b = b.split(' ')
    for i in range(n):
        b[i] = float(b[i])
    b = np.array(b)
    xita = input('输入误差：')
    xita = float(xita)
    maxM = input('输入最大步数')
    maxM = int(maxM)
    Jacobi(A,b,xita,maxM)
#A = np.array([[2,-1,0,0],[-1,2,-1,0],[0,-1,2,-1],[0,0,-1,2]])
#b = np.array([1,0,1,0])
# A1 = np.array([[20,2,3],[1,8,1],[2,-3,15]])
# b1 = np.array([24,12,30])
# Jacobi(A1,b1,0.000001,1000)
if __name__ == '__main__':
    main()