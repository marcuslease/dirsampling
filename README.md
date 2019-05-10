# dirsampling
a Max object to get random samplings of audio files from a directory

## todo
* start using dirsampling_dir_hash global var to store results of dir walk;
stop using dirsampling_nftw_me/counter
  - dirsampling_dir() should:
    * perform dir walk and populate dirsampling_dir_hash
    * return sampling of files in dirsampling_dir_hash
* ignore non-audio files
* randomize sampling
