'''
	Data_Analysis.py
	Created on: Nov 25th, 2018
       Author: dreifuerstrm
	
	Analyzes the data collected from the Arduino and performs preliminary statistics on the features.
	A number of different machine learning models are considered using k-folds on a train, validation, and test set. Through initial investigations a decision tree was found to be an optimal design, especially after using SMOTE to over sample the fall data to give the models an even distribution of falling vs not falling. The results of a DT for the validation and test sets are then shown in a confusion matrix
	   
	   Last updated 12/20/18
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


seed = 10                           # sets the random seed value
trees = 5

# load dataset
#columns = ["num", "dx", "dy", "dz","ddx", "ddy", "ddz","dddx", "dddy", "dddz", "accelx", "accely", "accelz", "danger"]
dataset = pd.read_csv("C:\Users\dreifuerstrm\SeniorDesign\Code\Arduino\All_Data.csv")
dataset = dataset.drop(["Num"], axis=1)
dataset = dataset.apply(pd.to_numeric)
print(dataset.head())
#new_dataset = SelectKBest(k=6).fit_transform(dataset.drop(['danger'], axis=1), dataset.danger)
#print(pd.DataFrame(new_dataset).head())

dataset = dataset.drop(["dddx", "dddy", "dddz","ddx", "ddy", "ddz"], axis = 1)
names = dataset.columns
print(names)

'''
max_abs_scaler = MinMaxScaler()
dataset = max_abs_scaler.fit_transform(dataset)
dataset = pd.DataFrame(dataset, columns=names)
print(dataset.head())
'''


graphs = raw_input("would you like graphs? 1 = yes, else no")
if graphs == '1':
    #basic statistics for the dataset
    print(dataset.describe())

    # box and whisker plots of each variable
    dataset.plot(kind = 'box', subplots=True, layout = (4,4), sharex=False, sharey = False)
    plt.show()

    # histogram
    dataset.hist()
    plt.show()

    # scatter plot matrix
    scatter_matrix(dataset)
    plt.show()

# split the datat into training and testing
x = dataset.drop(['danger'], axis = 1)
col = x.columns
y = dataset.danger


validation_size = 0.20             # 20% test size
x_train, x_test, y_train, y_test = model_selection.train_test_split(x, y, test_size = validation_size, random_state = seed)
sm = SMOTE(random_state = seed)
x_train, y_train = sm.fit_resample(x_train, y_train)
x_train, x_valid, y_train, y_valid = model_selection.train_test_split(x_train, y_train, test_size = validation_size, random_state= seed)

seed = 7 # must be reset each time
scoring = 'accuracy'

models = []
models.append(('LR', LogisticRegression(solver='lbfgs')))
models.append(('LDA', LinearDiscriminantAnalysis()))
models.append(('KNN', KNeighborsClassifier()))
models.append(('DT', DecisionTreeClassifier(max_depth=8, min_samples_leaf=8)))
models.append(('NB', GaussianNB()))

tuning = raw_input('would you like to tune parameters? 1 for yes, else no')
if(tuning == '1'):
    for i in range(10):
        models.append(('GBM{},{}'.format((i+2)*18, 2), GradientBoostingClassifier(n_estimators=(i+2)*18, max_depth=2, random_state=seed)))


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
clf = RandomForestClassifier(n_estimators=trees, max_depth=6, min_samples_leaf=8, random_state=seed) #, class_weight={0:0.4, 1:0.2})
#clf = DecisionTreeClassifier( random_state=seed, min_samples_leaf=0.1)
clf.fit(x_train, y_train)

print("----------------------Train Results--------------------------")
pred = clf.predict(x_train)
print(accuracy_score(y_train, pred))
print(confusion_matrix(y_train, pred))
print(classification_report(y_train, pred))

print("----------------------Valid Results--------------------------")
predictions = clf.predict(x_valid)
print(accuracy_score(y_valid, predictions))
print(confusion_matrix(y_valid, predictions))
print(classification_report(y_valid, predictions))


print("----------------------Test Results--------------------------")
test_pred = clf.predict(x_test)
print(model_selection.cross_val_score(clf, x_test, y_test, cv=5))
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
    fpr[i], tpr[i], _ = roc_curve(y_train, pred)
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
plt.title('Receiver operating characteristic of DT')
plt.legend(loc="best")
plt.show()

def tree_to_code(tree, feature_names):
    tree_ = tree.tree_
    feature_name = [
        feature_names[i] if i != _tree.TREE_UNDEFINED else "undefined!"
        for i in tree_.feature
    ]
    print "def tree({}):".format(", ".join(feature_names))

    def recurse(node, depth):
        indent = "  " * depth
        if tree_.feature[node] != _tree.TREE_UNDEFINED:
            name = feature_name[node]
            threshold = tree_.threshold[node]
            print "{}if {} <= {}:".format(indent, name, threshold)
            recurse(tree_.children_left[node], depth + 1)
            print "{}else:  # if {} > {}".format(indent, name, threshold)
            recurse(tree_.children_right[node], depth + 1)
        else:
            print "{}return {}".format(indent, tree_.value[node])

    recurse(0, 1)

