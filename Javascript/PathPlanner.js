function PathPlanner() 
{
	this.nodes = []; 
	this.costs = []; 
	this.goal = [];
	this.a = [];
	this.b = [];
	this.portals = [];
}

PathPlanner.prototype = 
{
	best: function()
	{
		for (var i in this.nodes)
			return i;
	},

	triarea: function(a, b, c) 
	{
		return ((b.x-a.x)*(c.y-a.y))-((b.y-a.y)*(c.x-a.x));
	},

	distance: function(a, b)
	{
		return Math.sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
	},

	locate: function(a)
	{
		var i = 0;
		for (var n=0; n<3; n++)
			if (this.triarea(level[level[i].next].origin, level[i].origin, a) > 0)
				i = level[i].pair,
				n = -1;
			else 
				i = level[i].next;
		return [i, level[i].next, level[level[i].next].next];
	},

	middle: function(i)
	{
		var x = (level[i].origin.x+level[level[i].next].origin.x)/2;
		var y = (level[i].origin.y+level[level[i].next].origin.y)/2;
		return {'x':x,'y':y};
	},

	addNode: function(i, best)
	{
		var mid = this.middle(i);
		var costG = this.nodes[best].costG + this.distance(this.middle(this.nodes[best].index), mid); 
		var costF = Math.floor((costG + this.distance(mid,this.b))*1000000);

    	if (this.costs[[mid.x,mid.y]] == null)
			this.nodes[costF] = {'costG':costG,'index':i},
			this.costs[[mid.x,mid.y]] = {'cost':costF,'parent':this.nodes[best].index}; 
		
    	else if (this.costs[[mid.x,mid.y]].cost > costF)
    		delete this.nodes[this.costs[[mid.x,mid.y]].cost],
			this.nodes[costF] = {'costG':costG,'index':i},
			this.costs[[mid.x,mid.y]] = {'cost':costF,'parent':this.nodes[best].index};
	},

	search: function(a, b) 
	{
		this.a = a;
		this.b = b;
		
		this.goal = this.locate(b);

		for (var i of this.locate(a))
		{
			if (level[i].constrained == false)
			{						
				var mid = this.middle(i);
				var costG = this.distance(this.a, mid);
				var costF = Math.floor((costG + this.distance(mid,this.b))*1000000); 
      			this.nodes[costF] = {'costG':costG,'index':i};
            	this.costs[[mid.x,mid.y]] = {'parent':null};
			}
		}
		
		while (true)
		{
			var best = this.best(); 
			var i = level[level[this.nodes[best].index].pair].next;

			if (this.goal.indexOf(i) > -1)
			{
				var tmp = this.nodes[best].index;

				while (tmp != null)
				{
					this.portals.push({'left':level[level[tmp].next].origin,'right':level[tmp].origin});
					var mid = this.middle(tmp);
					tmp = this.costs[[mid.x,mid.y]].parent;
				}
				
				return this.funnel();
			}

			else
			{
				for (var n=0; n<2; n++, i = level[i].next)
					if (level[i].constrained == false)
						this.addNode(i, best);
					
				delete this.nodes[best];
			}
		}	
	},

	funnel: function()
	{
		var pts = [];

		var apex = this.b;
		var left = 0;
		var right = 0;

		pts.push(apex);

		for (var i = 1; i < Object.keys(this.portals).length; i++)  
		{
			if (this.triarea(apex, this.portals[i].right, this.portals[right].right) >= 0.0) 
			{
				if (this.triarea(apex, this.portals[i].right, this.portals[left].left) > 0.0)
					pts.push(this.portals[left].left),
					apex = this.portals[left].left,							
					i = left;
				right = i;
			}
				
			if (this.triarea(apex, this.portals[left].left, this.portals[i].left) >= 0.0) 
			{
				if (this.triarea(apex, this.portals[right].right, this.portals[i].left) > 0.0)			
					pts.push(this.portals[right].right),
					apex = this.portals[right].right,
					i = right;	
				left = i;	
			}
		}

		if (this.triarea(apex, this.portals[right].right, this.a) > 0)
		{
			while (right != Object.keys(this.portals).length)
			{
				if (this.triarea(apex, this.portals[right].right, this.a) > 0)
					pts.push({'x':this.portals[right].right.x,'y':this.portals[right].right.y}),
					apex = this.portals[right].right;
				right++;
			}
		}

		else if (this.triarea(apex, this.a, this.portals[left].left) > 0)
		{
			while (left != Object.keys(this.portals).length)
			{
				if (this.triarea(apex, this.a, this.portals[left].left) > 0)
					pts.push({'x':this.portals[left].left.x,'y':this.portals[left].left.y}),
					apex = this.portals[left].left;
				left++;
			}
		}	

		pts.push(this.a);

		return pts;
	},

	drawMesh: function()
	{
		var c = document.getElementById("myCanvas");
		var ctx = c.getContext("2d");

		for (var i=0; i<level.length; i++)
		{
			ctx.beginPath();

			ctx.moveTo(level[i].origin.x, level[i].origin.y);
			ctx.lineTo(level[level[i].next].origin.x, level[level[i].next].origin.y);

			if (level[i].constrained == true)
				ctx.strokeStyle = '#ff0000';
			else
				ctx.strokeStyle = '#d3d3d3';

			ctx.stroke();
		}
	}	
};