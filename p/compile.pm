package tau::compile;


sub main
{
  my ($source) = @_;
  my $map = tau::map->new($source);
  printf "%d\n",     $map->n;
  printf "%d, %d\n", $map->next(0, 0, 's');
  printf "0, 1 = %s\n", chr $map->at(0, 1);
}
