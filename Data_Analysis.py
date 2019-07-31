'''
	Data_Analysis.py
	Created on: Nov 25th, 2018
       Author: dreifuerstrm
	
	Analyzes the data collected from the Arduino and performs preliminary statistics on the features.
	A number of different machine learning models are considered using k-folds on a train, validation, and test set. Through initial investigations a decision tree was found to be an optimal design, especially after using SMOTE to over sample the fall data to give the models an even distribution of falling vs not falling. The results of a DT for the validation and test sets are then shown in a confusion matrix
	   
	   Last updated 1/20/19
'''
# Load libraries
from pandas.plotting import scatter_matrix
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn import model_selection
from sklearn.metrics import classification_report
from sklearn.metrics import confusion_matrix
from sklearn.metrics import accuracy_score
from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.naive_bayes import GaussianNB
from sklearn.preprocessing import MinMaxScaler
from imblearn.over_sampling import SMOTE
from sklearn.tree import _tree
from sklearn import preprocessing, model_selection
from sklearn.metrics import roc_curve, auc
from sklearn.tree import export_graphviz
from sklearn.ensemble import GradientBoostingClassifier
import xgboost as xgb


seed = 12                           # sets the random seed value
trees = 5
depth = 8

# load dataset

dataset = pd.read_csv(r"C:\Users\Ryan_Dreifuerst\BT5_Dev\Data\All_Data.csv")
dataset = dataset.apply(pd.to_numeric)
print(dataset.head())
#new_dataset = SelectKBest(k=6).fit_transform(dataset.drop(['danger'], axis=1), dataset.danger)
#print(pd.DataFrame(new_dataset).head())

names = dataset.columns
print(names)

'''
max_abs_scaler = MinMaxScaler()
dataset = max_abs_scaler.fit_transform(dataset)
dataset = pd.DataFrame(dataset, columns=names)
#print(dataset.head())
'''


graphs = input(r"would you like graphs? 1 = yes, else no")
if graphs == '1':
    #basic statistics for the dataset
    print(dataset.describe())

    # box and whisker plots of each variable
    dataset.plot(kind = 'box', subplots=True, layout = (5,4), sharex=False, sharey = False)
    plt.show()

    # histogram
    dataset.hist()
    plt.show()

    # scatter plot matrix
    scatter_matrix(dataset)
    plt.show()

# split the datat into training and testing
x = dataset.drop(['Danger'], axis = 1)
col = x.columns
y = dataset.Danger


validation_size = 0.15             # 15% test size
x_train, x_test, y_train, y_test = model_selection.train_test_split(x, y, test_size = validation_size, random_state = seed)
smo = input(r'Would you like oversampling?')
if(smo == '1'):
    sm = SMOTE(random_state = seed)
    x_train, y_train = sm.fit_resample(x_train, y_train)
x_train, x_valid, y_train, y_valid = model_selection.train_test_split(x_train, y_train, test_size = validation_size, random_state= seed)

scoring = 'accuracy'

models = []
models.append(('LR', LogisticRegression(solver='lbfgs', max_iter = 4000)))
models.append(('LDA', LinearDiscriminantAnalysis()))
models.append(('KNN', KNeighborsClassifier()))
models.append(('DT', DecisionTreeClassifier(max_depth=8, min_samples_leaf=8)))
models.append(('NB', GaussianNB()))

tuning = input(r'would you like to tune parameters? 1 for yes, else no')
if(tuning == '1'):
    for i in range(10):
        zero = 0.1+i/10.0
        one = 0.8
        models.append(('RF{},{}'.format(i+1, i*3), RandomForestClassifier(n_estimators=trees,
                                                                           max_depth=i+1, min_samples_leaf=(i+1)*3,
                                                                           random_state=seed,
                                                                           )))
'''
dmatrix = xgb.DMatrix(data=x_train, label=y_train, feature_names=col, )
d_validation = xgb.DMatrix(data=x_valid, label=y_valid, feature_names=col,)
d_test = xgb.DMatrix(data=x_test, label=y_test, feature_names=col,)
'''


results = []
names = []
for name, model in models:
	# test harness wil split data into 10 parts
    kfold = model_selection.KFold(n_splits=10, random_state = seed)
    cv_results = model_selection.cross_val_score(model, x_train, y_train, cv=kfold, scoring=scoring)
    results.append(cv_results)
    names.append(name)
    msg = "%s: %f (%f)" % (name, cv_results.mean(), cv_results.std())
    print(msg)

# next decide which is best. Then run a full accuracy score, confusion matrix, and classification report
clf = RandomForestClassifier(n_estimators=trees, max_depth=depth, min_samples_leaf=5, random_state=seed) #, class_weight={0:0.4, 1:0.2})
'''
param = {'max_depth' : 7, 'objective' :'binary:logistic', 'n_estimators': trees, 'eval_metric':'auc', }
evallist = [(d_test, 'eval'), (dmatrix, 'train')]
clf = xgb.train(param, dmatrix, 10, evallist)
'''
clf.fit(x_train, y_train)

print("----------------------Train Results--------------------------")
pred = clf.predict(x_train)
#pred = [round(value) for value in pred]
print(accuracy_score(y_train, pred))
print(confusion_matrix(y_train, pred))
print(classification_report(y_train, pred))

print("----------------------Valid Results--------------------------")
predictions = clf.predict(x_valid)
#predictions = [round(value) for value in predictions]
print(accuracy_score(y_valid, predictions))
print(confusion_matrix(y_valid, predictions))
print(classification_report(y_valid, predictions))


print("----------------------Test Results--------------------------")
test_pred = clf.predict(x_test)
#test_pred = [round(value) for value in test_pred]
print(model_selection.cross_val_score(clf, x_test, y_test, cv=8))
print(accuracy_score(y_test, test_pred))
print(confusion_matrix(y_test, test_pred))
print(classification_report(y_test, test_pred))

if(type(clf) == type(DecisionTreeClassifier())):
    with open("Wrist_Rescue.dot", 'w') as f:
        f = export_graphviz(clf, out_file=f, feature_names=col,
                        filled=True, rounded=True)
else:
    if(type(clf) == type(RandomForestClassifier())):
        for i in range(trees):
            with open("Wrist_Rescue_{}.dot".format(i), 'w') as f:
                f = export_graphviz(clf.estimators_[i], out_file=f, feature_names=col,
                                filled=True, rounded=True)

# to generate plot:
# at the command line, run this to convert to PNG:
# must cd to the correct folder!
# dot -Tpng Wrist_Rescue.dot -o Wrist_rescue.png



fpr = dict()
tpr = dict()
roc_auc = dict()
for i in range(2):
    fpr[i], tpr[i], _ = roc_curve(y_test, test_pred)
    roc_auc[i] = auc(fpr[i], tpr[i])

plt.figure()
lw = 2
plt.plot(fpr[1], tpr[1], color='darkorange',
         lw=lw, label='ROC curve (area = %0.2f)' % roc_auc[1])

plt.plot([0, 1], [0, 1], color='navy', lw=lw, linestyle='--')
plt.xlim([0.0, 1.0])
plt.ylim([0.0, 1.05])
plt.xlabel('False Positive Rate')
plt.ylabel('True Positive Rate')
plt.title('Receiver operating characteristic of RF')
plt.legend(loc="best")
plt.show()

global brackets
brackets = 0
    
def tree_to_code(tree, feature_names):
    tree_ = tree.tree_

    feature_name = [
        feature_names[i] if i != _tree.TREE_UNDEFINED else "undefined!"
        for i in tree_.feature
    ]
    print("def tree({}):".format(", ".join(feature_names)))

    def recurse(node, depth, brackets):
        indent = "  " * depth
        if tree_.feature[node] != _tree.TREE_UNDEFINED:
            name = feature_name[node]
            threshold = int(np.round(tree_.threshold[node]))
            brackets += 1
            print("{}if({} <= {}) {{".format(indent, name, threshold))
            recurse(tree_.children_left[node], depth + 1, brackets)
            print("{}else {{  // if {} > {}".format(indent, name, threshold))
            recurse(tree_.children_right[node], depth + 1, brackets)
        else:
            high_val = max(tree_.value[node][0])
            brackets -= 1
            print("{}return {}; }}".format(indent, np.where(tree_.value[node][0]==high_val)[0][0]), "\n {}".format(indent))
    brackets = 0
    recurse(0, 1, 0)

code = input(r"would you like code? 1 = yes, else no")
if(code=='1'):
    for i in range(trees):
            print("/*********** Tree {} *************/".format(i))
            tree_to_code(clf.estimators_[i], col)

def predict(xavg, yavg, zavg, dx, dy, dz, x, y, z):
    print(clf.predict(np.array([xavg, yavg, zavg, dx, dy, dz, x, y, z]).reshape(1, -1)))
    
