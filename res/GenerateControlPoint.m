function result = GenerateControlPoint(prevpoint, point, nextpoint, givenext)
	% Lasken kontrollipisteet nyt niin että viivan tangentti on suora
	% pistettä edellisestä interpolaatiopisteestä sitä seuraavaan
	result = nextpoint - prevpoint;
	result = result/norm(result);

	% katotaan suunta, - edelliseen, + seuraavaan
	% painotetaan kanssa kontrollipisteen suuruutta
	% polun pituuden neliöjuurella, mutta tämä on	vaan randomi valinta, voi olla mitä vaan
	if(givenext==true)
		dist = (nextpoint-point);
		dist = sqrt(norm(dist));
		result = result*dist;
	else
		dist = (point-prevpoint);
		dist = sqrt(norm(dist));
		result = -result*dist;
	end
end