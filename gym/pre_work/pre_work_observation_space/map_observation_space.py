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
                if "%s" not in word \
                   and "%i" not in word \
                   and "monsters" not in word \
                   and "gibs" not in word \
                   and "world" not in word \
                   and "tank" not in word \
                   and "soldier" not in word \
                   and "sprites" not in word \
                   and "mutant" not in word \
                   and "ships" not in word \
                   and "keys" not in word \
                   and "deadbods" not in word \
                   and "makron" not in word \
                   and "medic" not in word \
                   and "infantry" not in word \
                   and "hover" not in word \
                   and "gladiator" not in word \
                   and "gunner" not in word \
                   and "flyer" not in word \
                   and "flipper" not in word \
                   and "floater" not in word \
                   and "chick" not in word \
                   and "brain" not in word \
                   and "boss" not in word \
                   and "berserk" not in word \
                   and "insane" not in word \
                   and "menu" not in word \
                   and "talk" not in word:
                    self.observations[word] = None

    def _write_observations(self, filename):
        observations = list(self.observations.keys())
        observations_file = open("observations.txt", "w")
        observations.sort()
        for observation in observations:
            observations_file.write("%s\n" % (observation,))
            
map_observation_space("observations.txt")

    
