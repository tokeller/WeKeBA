M = csvread('MehrereEinschlaege_edit.CSV');
N = M(:,[1,4]);
clear('M');
%%
plot(N(:,1),N(:,2));
figure(1)
title('Mehrere Einschläge auf einer Aluplatte. ');
xlabel('Zeit /[s]');
ylabel('Amplitude / [V]');