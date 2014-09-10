M = csvread('MehrereEinschlaege_edit.CSV');
N = M(:,[1,4]);
clear('M');
plot(N(:,1),N(:,2));