package tau::compile;


sub main
{
  my ($source) = @_;
  my $map = tau::map->new($source);
  printf "%d\n", $map->n;

}
