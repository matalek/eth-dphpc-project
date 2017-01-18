class Algorithm:
    def __init__(self, algorithm_name):
        self.name = algorithm_name

        # Map from number of points to array of execution times
        self.execution_time = {}
        self.mid_execution_time = {}
        self.end_execution_time = {}
