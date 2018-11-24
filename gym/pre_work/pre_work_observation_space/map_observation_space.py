class map_observation_space:
    def __init__(self, filename):
        self.observations = {}
        self.sounds_file, self.models_file = self._read()
        self._parse(self.sounds_file)
        self._parse(self.models_file)
        self._write_observations(filename)
        
    def _read(self):
        sounds_file = open("sounds.txt", "r")
        models_file = open("models.txt", "r")
        return sounds_file, models_file

    def _parse(self, file_):
        for line in file_:
            for word in line.split():
                if "%s" not in word and "%i" not in word:
                    self.observations[word] = None

    def _write_observations(self, filename):
        observations = list(self.observations.keys())
        observations_file = open("observations.txt", "w")
        observations.sort()
        for observation in observations:
            observations_file.write("%s\n" % (observation,))
            
map_observation_space("observations.txt")

    
