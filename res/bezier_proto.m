close all;
clear;

interp_p = 20;

points = [0 0;
	  10 1;
	  10 5;
	  -10 2;
	  0 10];

% 1:2 on edellisen suuntaan, 3:4 on seuraavan suuntaan
for ii=2:(size(points,1) -1)
	controlpoints(ii,1:2) = GenerateControlPoint(points(ii-1,:), points(ii,:), points(ii+1,:), false);
	controlpoints(ii,3:4) = GenerateControlPoint(points(ii-1,:), points(ii,:), points(ii+1,:), true);
end

% N‰‰ on p‰‰tepisteet
controlpoints(1,1:2) = [0 0];
controlpoints(1,3:4) = (points(2,:) - points(1,:))/norm((points(2,:) - points(1,:)));
controlpoints(size(points,1),1:2) = (points(size(points,1)-1,:) - points(size(points,1),:))/norm((points(size(points,1)-1,:) - points(size(points,1),:)));
controlpoints(size(points,1),3:4) = [0 0];

% k‰yt‰ edellisi‰ ja laske pisteet
for ii=1:(size(points,1)-1)
	for jj=1:interp_p
		cp1 = controlpoints(ii,3:4);
		cp2 = controlpoints(ii+1,1:2);
		result((ii-1)*interp_p + jj,:) = DrawBezierPoint(points(ii,:), cp1, points(ii+1,:), cp2, jj/interp_p);
	end
end

% lÈ print
figure
hold on
plot(points(:,1) + controlpoints(:,1), points(:,2) + controlpoints(:,2),'g.')
plot(points(:,1) + controlpoints(:,3), points(:,2) + controlpoints(:,4),'b.')
plot(result(:,1),result(:,2),'r.')
plot(points(:,1), points(:,2),'k.')
hold off