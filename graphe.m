close('all');
filename = 'timesfile';
fichier = importdata([filename,".txt"]," ",1);
sizes = fichier.data(:,1);
temps = fichier.data(:,2);
#debits = fichier.data(:,3);
hold on;
%plot(sizes,temps);
loglog(sizes,temps);
p = polyfit(log(sizes),log(temps),1);
pente=p(1)
xlabel('nombre de villes');
ylabel('temps en secondes');
title({["pente = ",num2str(pente)]});
print("tempsloglog",'-djpeg');
