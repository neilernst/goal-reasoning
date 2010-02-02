from string import *
__all__ = ['NFR', 'SoftGoal']

class Model:
	def sd_and(self,a,b):
		return abs(a)*abs(b)
	def sd_or(self,a,b):
		return abs(a)+abs(b)-abs(a)*abs(b)
	def sd_inv(self,a):
		return 1-abs(a)

class NFR:
	"The non-functional requirement framework"
	num_soft_goals = 0
	num_relationships = 0
	def __init__(self, name):
		self.name = name
		self.list = []
		
	def add(self, goal):
		self.list.append(goal)
		
	def report(self):
		print "%d soft goals are read" % NFR.num_soft_goals
		print "%d relationships are read" % NFR.num_relationships
		for x in self.list:
			x.report()
			
	def report_for_dot(self):
		print "digraph nfrgraph {"
		for x in self.list:
			x.report_for_dot()
		print "}"
		
	def find_goal(self, key):
		for x in self.list:
			if x.is_goal():
				if x.key == key:
				   return x
		print "goal %s not found" % key
		
	def read(self, name):
		f = open(name, 'r')
		for x in f.readlines():
			if x[len(x)-1]=='\n':
				fields = split(x[:-1], ':') # get rid of \n at the end of the line
			else:
				fields = split(x, ':')
			if (len(fields)==3):
				if (fields[2]=="and" or fields[2]=="or"):
					a = self.find_goal(fields[0])
					b = self.find_goal(fields[1])
					relationship = Relationship(a, b, fields[2], 0, 0)
					self.add(relationship)
					NFR.num_relationships = NFR.num_relationships + 1
				else:
					goal = SoftGoal(fields[0], fields[1], fields[2])
					self.add(goal)
					NFR.num_soft_goals = NFR.num_soft_goals + 1
			else:
					a = self.find_goal(fields[0])
					b = self.find_goal(fields[1])
					relationship = Relationship(a, b, "", fields[2], fields[3])
					self.add(relationship)
					NFR.num_relationships = NFR.num_relationships + 1
					
	def read_dat(self, name):
		f = open(name, 'r')
		for x in f.readlines():
			if x[len(x)-1]=='\n':
				fields = split(x[:-1]) # get rid of \n at the end of the line
			else:
				fields = split(x)
			if (fields[0]=="N"): # Node
				if (fields[1]=="NO"): 
					s = 0
				elif (fields[1] == "PARTIAL"):
					s = 0.5
				elif (fields[1] == "TOTAL"):
					s = 1
				else:
					s = float(fields[1])
				if (fields[1]=="NO"): 
					d = 0
				elif (fields[1] == "PARTIAL"):
					d = 0.5
				elif (fields[1] == "TOTAL"):
					d = 1
				else:
					d = float(fields[2])
				goal = SoftGoal("%d"%NFR.num_soft_goals, s, d)
				self.add(goal)
				NFR.num_soft_goals = NFR.num_soft_goals + 1
			elif (fields[0] == "R"): # Rules for edges
				if (fields[1] == "AND" or fields[1] == "OR"):
					b = self.find_goal(fields[2])
					for i in range(3, len(fields)):
						a = self.find_goal(fields[i])
						relationship = Relationship(a, b, fields[1], 0, 0)
						self.add(relationship)
						NFR.num_relationships = NFR.num_relationships + 1
				else: # single contribution rule
					b = self.find_goal(fields[2])
					a = self.find_goal(fields[3])
					fs = split(fields[1], "%")
					op = fs[0]
					s = 0
					d = 0
					if len(fs)==2:
						opr = float(fs[1])
					else:
						opr = 0.5 * len(op)
					if   (op[0]=="+"):
						s = opr
					elif (op[0]=="*"):
						d = opr
					elif (op[0]=="-"):
						s = -opr
					elif (op[0]=="/"):
						d = -opr
					relationship = Relationship(a, b, "", s, d)
					self.add(relationship)
					NFR.num_relationships = NFR.num_relationships + 1
					
	def calculate(self, g):
		m = Model()
		g.new_deny = g.deny
		g.new_satisfy = g.satisfy
		and_list = []
		for f in self.list:
			if f.is_goal()==0 and f.target==g:
				if f.type == "AND":
					and_list.append(f);
		if len(and_list)>1:
			s = 1; d = 0;
			for f in and_list:
				s = m.sd_and(s, f.source.satisfy)
				d = m.sd_or(d, f.source.deny)
			g.new_satisfy = s
			g.new_deny = d
		else:
			or_list = []
			for f in self.list:
				if f.is_goal()==0 and f.target==g:
					if f.type == "OR":
						or_list.append(f);
			if len(or_list)>1:
				s = 0; d = 1;
				for f in or_list:
					s = m.sd_or(s, f.source.satisfy)
					d = m.sd_and(d, f.source.deny)
				g.new_satisfy = s
				g.new_deny = d
			else:
				for f in self.list:
					if f.is_goal()==0 and f.target==g:
						if f.satisfy > 0:
							g.new_satisfy = m.sd_or(g.new_satisfy, m.sd_and(f.source.satisfy, f.satisfy))
						elif f.satisfy < 0:
							g.new_satisfy = m.sd_and(g.new_satisfy, m.sd_or(f.source.satisfy, f.satisfy))
						if f.deny > 0:
							g.new_deny = m.sd_and(g.new_deny, m.sd_and(f.source.satisfy, f.satisfy))
						elif f.deny < 0:
							g.new_deny = m.sd_or(g.new_deny, m.sd_or(f.source.satisfy, f.satisfy))
		
	def label_propagation(self):
		changed = 1
		time = 1
		while (changed):
			changed = 0
			for g in self.list:
				if g.is_goal():
					self.calculate(g)
					if g.changed():
						changed = 1
			if changed:
				for g in self.list:
					if g.is_goal():
						g.update()
				time = time + 1
				
class SoftGoal:
	"""A soft goal of non-functional requirement framework"""
	
	def __init__(self, key, init_s, init_d):
		self.key = key
		self.new_satisfy = self.satisfy = float(init_s)
		self.new_deny = self.deny = float(init_d)
		
	def report(self):
		if (self.satisfy!=0 or self.deny!=0):
			print "%-30s %f %f" % (self.key, self.satisfy, self.deny)
			
	def report_for_dot(self):
		if (self.satisfy!=0 or self.deny!=0):
			print '"%s" [label="%s\\nS:%f,D:%f",shape=box]' % (self.key, replace(self.key," ", "\\n"), self.satisfy, self.deny)
		else:
			print '"%s" [label="%s"]' % (self.key, replace(self.key," ", "\\n"))
			
	def is_goal(self):
		return 1
		
	def changed(self):
		if (abs(self.new_satisfy-self.satisfy)<0.05 and abs(self.new_deny-self.deny)<0.05):
			return 0
		return 1
		
	def update(self):
		self.satisfy = self.new_satisfy
		self.deny = self.new_deny
		
class Relationship:
	""""The relationship from soft goal A to soft goal B"""
	
	def __init__(self, a, b, type, satisfy_factor, deny_factor):
		self.source = a
		self.target = b
		self.type = type # one of "and", "or", ""
		self.satisfy = float(satisfy_factor) # within range [-1, 1]
		self.deny = float(deny_factor) # within range [-1, 1]
	
	def report(self):
		print "%-30s -(%s)-> %-30s" % (self.source.key, self.get_label(), self.target.key)
	
	def get_label(self):
		if self.type == "AND" or self.type == "OR":
			return self.type
		else:
			if (self.deny!=self.satisfy):
				if (self.satisfy!=0):
					ann = "S"
				else:
					ann = "D"
			else:
				ann = ""
			if (self.deny>0 or self.satisfy>0):
				sign = "+"
			else:
				sign = "-"
			return "%f%s%s" % (max(abs(self.satisfy), abs(self.deny)), sign, ann)
	
	def report_for_dot(self):
		print '"%s" -> "%s" [label="%s"];' % (self.source.key, self.target.key, self.get_label())
	
	def is_goal(self):
		return 0
		
if __name__ == '__main__':
	g = NFR("GM")
	g.read_dat(r'nfr.dat')
	g.label_propagation()
	g.report_for_dot()
