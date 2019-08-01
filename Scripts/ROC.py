import pandas as pd
import numpy as np
from sklearn import model_selection
from sklearn.linear_model import SGDClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, recall_score, f1_score, precision_score
import matplotlib.pyplot as plt
import time
import sklearn.metrics as metrics
import matplotlib.pyplot as plt



df_train = pd.read_csv('training.data', header=None)
X_test = pd.read_csv('testing.data', header=None)
labels = pd.read_csv('result.data', header=None)

X_train = df_train.drop([9], axis=1)
y_train = df_train[9]

original_logistic_reg = SGDClassifier(loss='log', penalty='none')
original_logistic_reg.fit(X_train, y_train)
original_logistic_reg.coef_ = np.array([[0.312394, 0.361258, 0.779467, 2.39999, 0.652923, 1.82748, 0.558902, 0.584805, -0.0735222]])
original_logistic_reg.intercept_ = -2.6805

original_probs = original_logistic_reg.predict_proba(X_test)
original_preds = original_probs[:,1]
original_fpr, original_tpr, original_threshold = metrics.roc_curve(labels, original_preds)
original_roc_auc = metrics.auc(original_fpr, original_tpr)


approx_logistic_reg = SGDClassifier(loss='log', penalty='none')
approx_logistic_reg.fit(X_train, y_train)
approx_logistic_reg.coef_ = np.array([[0.376273, 0.444687, 0.930967, 2.86425, 0.725512, 2.18232, 0.58402, 0.71752, -0.111162]])
approx_logistic_reg.intercept_[0] = -3.28125

approx_probs = approx_logistic_reg.predict_proba(X_test)
approx_preds = approx_probs[:,1]
approx_fpr, approx_tpr, approx_threshold = metrics.roc_curve(labels, approx_preds)
approx_roc_auc = metrics.auc(approx_fpr, approx_tpr)



plt.title('Receiver Operating Characteristic')
plt.plot(original_fpr, original_tpr, 'b', label = 'Original AUC = %0.2f' % original_roc_auc)
plt.plot(approx_fpr, approx_tpr, 'g', label = 'Approx AUC = %0.2f' % approx_roc_auc)

plt.legend(loc = 'lower right')
plt.plot([0, 1], [0, 1],'r--')
plt.xlim([0, 1])
plt.ylim([0, 1])
plt.ylabel('True Positive Rate')
plt.xlabel('False Positive Rate')
plt.show()
