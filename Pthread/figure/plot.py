import matplotlib.pyplot as plt

# 數據
threads = [1, 2, 3, 4, 5, 6]
speedup = [1.00, 1.63, 2.13, 2.51, 2.89, 3.25]

# 繪製折線圖
plt.plot(threads, speedup, marker='o')

# 設定標題和標籤
plt.title('Speedup vs Threads')
plt.xlabel('Speedup')
plt.ylabel('Threads')

# 反轉Y軸以便於閱讀
# plt.gca().invert_yaxis()

# 顯示圖形
plt.grid()
plt.show()