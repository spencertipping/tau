package tau::compile;


sub main
{
  my ($source) = @_;
  my $map = tau::map->new($source);
  printf "%d\n",         $map->n;
  printf "%d, %d, %s\n", $map->next(0, 0, 's');
  printf "%d, %d, %s\n", $map->next(0, 2, 'e');
}
