# -*- coding: utf-8 -*-
"""
Created on Thu Apr 14 20:37:36 2022

@author: stife
"""

from matplotlib import pyplot as plt
from scipy.interpolate import splev, splrep
import numpy as np

#%%

duration_sec = 30
threads = [1, 2, 6, 10]
latency_ms = [27.32, 23.38, 14.33, 29.16]
requests_per_sec = [357.11, 303.87, 214.44, 165.47]

plt.figure(figsize=(7.20, 4.80), dpi=100)

plt.scatter(threads, latency_ms)

# Сглаживающая кривая
spl = splrep(threads, latency_ms)
xint = np.linspace(threads[0], threads[-1], 40)
yint = splev(xint, spl)
plt.plot(xint, yint)

plt.xlabel("Число потоков")
plt.ylabel("Задержка, ms")
plt.title("Тестирование задержки.\nПродолжительность теста:  %d сек" % duration_sec)
plt.grid(True)

#%%
plt.figure(figsize=(7.20, 4.80), dpi=100)

plt.scatter(threads, requests_per_sec)

# Сглаживающая кривая
spl = splrep(threads, requests_per_sec)
xint = np.linspace(threads[0], threads[-1], 40)
yint = splev(xint, spl)
plt.plot(xint, yint)

plt.xlabel("Число потоков")
plt.ylabel("Запросов в секунду")
plt.title("Тестирование пропускной способности.\nПродолжительность теста:  %d сек" % duration_sec)
plt.grid(True)

