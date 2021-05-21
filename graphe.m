filename = 'timesfile';
fichier = importdata([filename,".txt"]," ",1);
sizes = fichier.data(:,1);
temps = fichier.data(:,2);
#debits = fichier.data(:,3);
hold on;
plot(sizes,temps,'DisplayName',filename);