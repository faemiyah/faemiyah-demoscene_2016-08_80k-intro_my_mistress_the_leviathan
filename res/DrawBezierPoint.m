function result = DrawBezierPoint(p1, p1_c, p2, p2_c, fraction)

	% Control points
	c1 = p1 + p1_c;
	c2 = p2 + p2_c;

	% Control point followers
	d1 = p1 + p1_c * fraction;
	d2 = p2 + p2_c - p2_c * fraction;
	
	% Triangle head follower
	t = c1 + (c2-c1)*fraction;
	
	% Triangle side followers
	t1 = d1 + (t-d1)*fraction;
	t2 = t + (d2-t)*fraction;
	
  result = t1 + (t2-t1)*fraction;

end